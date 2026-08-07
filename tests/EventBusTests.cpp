/**
 * @file EventBusTests.cpp
 * @author TV1 (Duong)
 * @brief Regression tests for EventBus observer lifetime (S6-TV1-20) and
 *        subscriber duplicate/growth safety (S6-TV1-21).
 *
 * Verifies:
 *   - subscribing the same observer twice does not grow the listener list;
 *   - an observer unsubscribed (or destroyed) by another observer's callback is
 *     NOT invoked for that notification;
 *   - repeated subscribe/unsubscribe over many state-lifetime cycles does not
 *     accumulate stale callbacks.
 */

#include <cassert>
#include <iostream>

#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "patterns/IObserver.h"

namespace {

struct SharedCounters {
    int aCalls = 0;
    int bCalls = 0;
};

class ObserverA : public IObserver {
public:
    explicit ObserverA(SharedCounters& c) : m_c(c) {}
    void onNotify(EventType /*event*/) override { ++m_c.aCalls; }
private:
    SharedCounters& m_c;
};

// ObserverB can, on request, unsubscribe ObserverA from inside its own callback,
// simulating one observer destroying/unsubscribing another during dispatch.
class ObserverB : public IObserver {
public:
    explicit ObserverB(SharedCounters& c, EventBus& bus, IObserver* other)
        : m_c(c), m_bus(bus), m_other(other), m_unsubscribeOtherOnHit(false) {}

    void setUnsubscribeOtherOnHit(bool v) { m_unsubscribeOtherOnHit = v; }

    void onNotify(EventType event) override {
        ++m_c.bCalls;
        if (m_unsubscribeOtherOnHit) {
            m_bus.unsubscribe(event, m_other);
        }
    }
private:
    SharedCounters& m_c;
    EventBus& m_bus;
    IObserver* m_other;
    bool m_unsubscribeOtherOnHit = false;
};

} // namespace

int main() {
    EventBus& bus = EventBus::getInstance();

    // --- 1. Subscribing the same observer twice does not create duplicate callbacks. ---
    {
        std::cout << "[RUNNING] testNoDuplicateSubscribers..." << std::endl;
        SharedCounters c;
        ObserverA a(c);

        bus.unsubscribe(EventType::PLAYER_DIED, &a); // clean slate
        for (int i = 0; i < 50; ++i) {
            bus.subscribe(EventType::PLAYER_DIED, &a);
        }
        c.aCalls = 0;
        bus.notify(EventType::PLAYER_DIED);
        assert(c.aCalls == 1); // exactly one callback despite 50 subscribes

        bus.unsubscribe(EventType::PLAYER_DIED, &a);
        std::cout << "[PASSED] testNoDuplicateSubscribers" << std::endl;
    }

    // --- 2. Observer removed by another observer's callback is not invoked. ---
    {
        std::cout << "[RUNNING] testNoCallbackAfterUnsubscribe..." << std::endl;
        SharedCounters c;
        ObserverA a(c);
        ObserverB b(c, bus, &a);

        bus.unsubscribe(EventType::GAME_PAUSED, &a);
        bus.unsubscribe(EventType::GAME_PAUSED, &b);

        // Order: A then B. B unsubscribes A from within its own callback, so A
        // (earlier in the list, already executed) is irrelevant here; to test the
        // "skip already-unsubscribed observer" path we instead make the FIRST
        // observer remove the SECOND, and verify the second is not called.
        bus.subscribe(EventType::GAME_PAUSED, &b);
        bus.subscribe(EventType::GAME_PAUSED, &a);
        // Now B runs first; set B to remove A.
        c.aCalls = 0;
        c.bCalls = 0;
        b.setUnsubscribeOtherOnHit(true);

        bus.notify(EventType::GAME_PAUSED);
        // B's callback ran; it removed A before the dispatch loop reached A.
        assert(c.bCalls == 1);
        assert(c.aCalls == 0); // A must NOT be called after being removed

        // Reset and clean up.
        b.setUnsubscribeOtherOnHit(false);
        bus.unsubscribe(EventType::GAME_PAUSED, &a);
        bus.unsubscribe(EventType::GAME_PAUSED, &b);
        std::cout << "[PASSED] testNoCallbackAfterUnsubscribe" << std::endl;
    }

    // --- 3. Many state-lifetime cycles do not leave stale subscribers behind. ---
    {
        std::cout << "[RUNNING] testNoSubscriberGrowth..." << std::endl;
        SharedCounters c;
        {
            ObserverB root(c, bus, nullptr); // (re)subscribe/unsubscribe in-place
        }

        // Simulate 200 PlayState enter/exit cycles: subscribe on enter, unsubscribe on exit.
        c.bCalls = 0;
        for (int i = 0; i < 200; ++i) {
            // A fresh observer each cycle then destroyed at end of cycle scope.
            ObserverB tmp(c, bus, nullptr);
            bus.subscribe(EventType::LEVEL_STARTED, &tmp);
            // (no notify here)
            bus.unsubscribe(EventType::LEVEL_STARTED, &tmp);
        }

        // If unsubscribe works, notifying LEVEL_STARTED must fire zero callbacks for tmp.
        bus.notify(EventType::LEVEL_STARTED);
        assert(c.bCalls == 0); // no stale observer was called

        std::cout << "[PASSED] testNoSubscriberGrowth" << std::endl;
    }

    std::cout << "All EventBus tests passed successfully!" << std::endl;
    return 0;
}
