/**
 * @file EventBus.cpp
 * @brief Singleton EventBus with move-only RAII subscriptions.
 */

#include "patterns/EventBus.h"

#include <algorithm>
#include <cstddef>
#include <unordered_map>
#include <utility>
#include <vector>

namespace eventbus_detail {

/** One logical observer/event registration shared by all duplicate tokens. */
struct SubscriptionLease {
    SubscriptionLease(std::weak_ptr<EventBusState> owner,
                      EventType subscribedEvent,
                      IObserver* subscribedObserver)
        : bus(std::move(owner)),
          event(subscribedEvent),
          observer(subscribedObserver) {}

    ~SubscriptionLease();

    void retain() noexcept { ++tokenCount; }
    void release() noexcept;
    void disconnect() noexcept;
    bool connected() const noexcept {
        return active && tokenCount != 0u && !bus.expired();
    }

    std::weak_ptr<EventBusState> bus;
    EventType event;
    IObserver* observer = nullptr;
    std::size_t tokenCount = 0u;
    bool active = true;
};

struct ListenerRecord {
    IObserver* observer = nullptr;
    std::weak_ptr<SubscriptionLease> lease;
};

/**
 * Shared state deliberately contains only weak lease records.  It therefore
 * never keeps a token alive, and token destruction can remove its record even
 * while a dispatch is iterating over a snapshot.
 */
struct EventBusState {
    std::unordered_map<EventType, std::vector<ListenerRecord>, EventTypeHash>
        listeners;

    void removeLease(EventType event,
                     IObserver* observer,
                     const SubscriptionLease* lease) noexcept;
    void disconnectObserver(EventType event, IObserver* observer) noexcept;
};

void EventBusState::removeLease(EventType event,
                                IObserver* observer,
                                const SubscriptionLease* lease) noexcept {
    auto eventIt = listeners.find(event);
    if (eventIt == listeners.end()) {
        return;
    }

    auto& records = eventIt->second;
    records.erase(
        std::remove_if(records.begin(), records.end(),
                       [observer, lease](const ListenerRecord& record) {
                           if (record.observer != observer) {
                               return false;
                           }
                           const auto recordLease = record.lease.lock();
                           return !recordLease || recordLease.get() == lease;
                       }),
        records.end());

    if (records.empty()) {
        listeners.erase(eventIt);
    }
}

void EventBusState::disconnectObserver(EventType event,
                                       IObserver* observer) noexcept {
    if (observer == nullptr) {
        return;
    }

    auto eventIt = listeners.find(event);
    if (eventIt == listeners.end()) {
        return;
    }

    // Copy the leases before disconnecting because disconnect() removes its
    // record from this same vector.
    std::vector<std::shared_ptr<SubscriptionLease>> leases;
    leases.reserve(eventIt->second.size());
    for (const ListenerRecord& record : eventIt->second) {
        if (record.observer != observer) {
            continue;
        }
        if (auto lease = record.lease.lock()) {
            leases.push_back(std::move(lease));
        }
    }

    for (const auto& lease : leases) {
        lease->disconnect();
    }

    // Expired records are harmless, but remove them eagerly for deterministic
    // listener ordering and to keep the state compact.
    eventIt = listeners.find(event);
    if (eventIt != listeners.end()) {
        auto& records = eventIt->second;
        records.erase(
            std::remove_if(records.begin(), records.end(),
                           [observer](const ListenerRecord& record) {
                               return record.observer == observer ||
                                      record.lease.expired();
                           }),
            records.end());
        if (records.empty()) {
            listeners.erase(eventIt);
        }
    }
}

void SubscriptionLease::disconnect() noexcept {
    if (!active) {
        return;
    }

    // Mark inactive before touching the bus.  A dispatch already holding a
    // temporary strong lease will observe this flag and skip future calls.
    active = false;
    if (auto state = bus.lock()) {
        state->removeLease(event, observer, this);
    }
}

void SubscriptionLease::release() noexcept {
    if (tokenCount == 0u) {
        return;
    }
    --tokenCount;
    if (tokenCount == 0u) {
        disconnect();
    }
}

SubscriptionLease::~SubscriptionLease() {
    disconnect();
}

} // namespace eventbus_detail

Subscription::Subscription(
    std::shared_ptr<eventbus_detail::SubscriptionLease> lease) noexcept
    : m_lease(std::move(lease)) {
    if (m_lease) {
        m_lease->retain();
    }
}

Subscription::~Subscription() noexcept {
    reset();
}

Subscription::Subscription(Subscription&& other) noexcept
    : m_lease(std::move(other.m_lease)) {}

Subscription& Subscription::operator=(Subscription&& other) noexcept {
    if (this != &other) {
        reset();
        m_lease = std::move(other.m_lease);
    }
    return *this;
}

void Subscription::reset() noexcept {
    if (!m_lease) {
        return;
    }
    m_lease->release();
    m_lease.reset();
}

bool Subscription::connected() const noexcept {
    return m_lease && m_lease->connected();
}

EventBus& EventBus::getInstance() {
    static EventBus instance;
    return instance;
}

EventBus::EventBus()
    : m_state(std::make_shared<eventbus_detail::EventBusState>()) {}

EventBus::~EventBus() noexcept {
    // Explicitly end the state before EventBus storage is reclaimed.  Tokens
    // retained by other objects then see an expired weak_ptr and remain safe.
    m_state.reset();
}

Subscription EventBus::subscribe(EventType event, IObserver* observer) {
    if (observer == nullptr || !m_state) {
        return {};
    }

    auto& records = m_state->listeners[event];
    for (auto recordIt = records.begin(); recordIt != records.end();) {
        auto lease = recordIt->lease.lock();
        if (!lease || !lease->active || lease->tokenCount == 0u) {
            recordIt = records.erase(recordIt);
            continue;
        }
        if (recordIt->observer == observer) {
            // Duplicate subscriptions share one logical lease.  The returned
            // token increments its independent token count in the constructor.
            return Subscription(std::move(lease));
        }
        ++recordIt;
    }

    auto lease = std::make_shared<eventbus_detail::SubscriptionLease>(
        std::weak_ptr<eventbus_detail::EventBusState>(m_state), event, observer);
    records.push_back(eventbus_detail::ListenerRecord{observer, lease});
    return Subscription(std::move(lease));
}

void EventBus::unsubscribe(EventType event, IObserver* observer) {
    if (m_state) {
        m_state->disconnectObserver(event, observer);
    }
}

void EventBus::notify(const GameEvent& event) {
    // Keep the state alive for the duration of this dispatch.  Individual
    // leases remain weakly connected to it and may disconnect re-entrantly.
    const auto state = m_state;
    if (!state) {
        return;
    }

    const auto eventIt = state->listeners.find(event.type);
    if (eventIt == state->listeners.end()) {
        return;
    }

    // A snapshot preserves deterministic subscription order and tolerates
    // vector erasure from callbacks.  Each record is revalidated immediately
    // before invocation so reset/unsubscribe by an earlier callback suppresses
    // the removed observer.
    const auto records = eventIt->second;
    for (const auto& record : records) {
        auto lease = record.lease.lock();
        if (!lease || !lease->active || lease->tokenCount == 0u ||
            record.observer == nullptr) {
            continue;
        }
        record.observer->onNotify(event);
    }
}

void EventBus::notify(EventType event) {
    notify(GameEvent{event});
}
