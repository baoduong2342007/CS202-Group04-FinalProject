/**
 * @file EventBus.cpp
 * @author TV1
 * @brief Singleton EventBus implementation with safe notification and debug tracer
 * @note Week 2
 * @note Sprint 6 — S6-TV1-20: verify observer is still subscribed before each callback
 */

#include "patterns/EventBus.h"

#include <algorithm>
#include <iostream>

// ============================================================
// PATTERN: Observer (with Singleton EventBus)
// Reason: Decouples game modules (Mario, enemies, HUD, SoundManager)
//         so they can react to events without direct dependencies.
//         Singleton ensures a single global event bus for the entire game.
// ============================================================

EventBus& EventBus::getInstance() {
    static EventBus instance;
    return instance;
}

void EventBus::subscribe(EventType event, IObserver* observer) {
    if (!observer) {
        return;
    }

    auto& observers = m_listeners[event];
    if (std::find(observers.begin(), observers.end(), observer) == observers.end()) {
        observers.push_back(observer);
    }
}

void EventBus::unsubscribe(EventType event, IObserver* observer) {
    if (!observer) {
        return;
    }

    auto it = m_listeners.find(event);
    if (it != m_listeners.end()) {
        auto& observers = it->second;
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }
}

void EventBus::notify(EventType event) {
    auto it = m_listeners.find(event);
    if (it == m_listeners.end()) {
        return;
    }

    // Copy the observer list to prevent iterator invalidation
    // if an observer calls unsubscribe() during onNotify().
    auto observersCopy = it->second;

#ifdef DEBUG
    std::cout << "[EventBus] Dispatched event ID: " << static_cast<int>(event)
              << " | Listeners notified: " << observersCopy.size() << "\n";
#endif

    for (auto* observer : observersCopy) {
        if (!observer) {
            continue;
        }

        // S6-TV1-20: Before invoking the callback, verify the observer is still
        // subscribed. A previous callback may have unsubscribed or destroyed it.
        auto currentIt = m_listeners.find(event);
        if (currentIt == m_listeners.end()) {
            break; // All listeners for this event were removed
        }
        const auto& currentObservers = currentIt->second;
        if (std::find(currentObservers.begin(), currentObservers.end(), observer) == currentObservers.end()) {
            continue; // Observer was unsubscribed during notification — skip it
        }

        observer->onNotify(event);
    }
}