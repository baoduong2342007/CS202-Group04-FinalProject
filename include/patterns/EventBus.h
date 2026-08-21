/**
 * @file EventBus.h
 * @author TV1
 * @brief Singleton EventBus implementation for global decoupled messaging
 * @note Week 2
 * 
 * @example Usage for Subscribing:
 *   auto token = EventBus::getInstance().subscribe(EventType::PLAYER_JUMPED, this);
 * 
 * @example Usage for Publishing:
 *   EventBus::getInstance().notify(EventType::PLAYER_JUMPED);
 * 
 */

#pragma once

#include <cstddef>
#include <memory>

#include "patterns/GameEvent.h"
#include "patterns/IObserver.h"
#include "patterns/ISubject.h"

/**
 * @brief Custom hash functor for EventType enum class for cross-compiler portability.
 */
struct EventTypeHash {
    std::size_t operator()(EventType event) const noexcept {
        return static_cast<std::size_t>(event);
    }
};

/**
 * @brief Global singleton event bus decoupling game entities, HUD, and audio managers.
 */
class EventBus : public ISubject {
public:
    // 1. Singleton access
    static EventBus& getInstance();

    // Deleted copy constructor & assignment operator for Singleton safety
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;

    // 2. Override methods
    [[nodiscard]] Subscription subscribe(EventType event,
                                         IObserver* observer) override;
    void unsubscribe(EventType event, IObserver* observer) override;
    void notify(const GameEvent& event) override;
    void notify(EventType event) override;

private:
    // Private constructor & destructor for Singleton
    EventBus();
    ~EventBus() noexcept override;

    std::shared_ptr<eventbus_detail::EventBusState> m_state;
};
