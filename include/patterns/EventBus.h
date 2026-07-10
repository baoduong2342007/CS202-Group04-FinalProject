/**
 * @file EventBus.h
 * @author TV1
 * @brief Singleton EventBus implementation for global decoupled messaging
 * @note Week 2
 * 
 * @example Usage for Subscribing:
 *   EventBus::getInstance().subscribe(EventType::PLAYER_JUMPED, this);
 * 
 * @example Usage for Publishing:
 *   EventBus::getInstance().notify(EventType::PLAYER_JUMPED);
 * 
 * @todo Week 4: Add deferred queueEvent() and std::any payload support for complex events.
 */

#pragma once

#include <unordered_map>
#include <vector>

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
    void subscribe(EventType event, IObserver* observer) override;
    void unsubscribe(EventType event, IObserver* observer) override;
    void notify(EventType event) override;

private:
    // Private constructor & destructor for Singleton
    EventBus() = default;
    ~EventBus() override = default;

    // Private members
    std::unordered_map<EventType, std::vector<IObserver*>, EventTypeHash> m_listeners;
};
