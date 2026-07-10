/**
 * @file ISubject.h
 * @author TV1
 * @brief Subject interface for publishing notifications to Observers
 * @note Week 2
 */

#pragma once

#include "patterns/EventType.h"

class IObserver;

/**
 * @brief Interface for event subjects capable of managing observer subscriptions.
 */
class ISubject {
public:
    virtual ~ISubject() = default;

    /**
     * @brief Subscribe an observer to a specific event type.
     * @param event The event type to listen to.
     * @param observer Pointer to the observer instance.
     */
    virtual void subscribe(EventType event, IObserver* observer) = 0;

    /**
     * @brief Unsubscribe an observer from a specific event type.
     * @param event The event type to stop listening to.
     * @param observer Pointer to the observer instance.
     */
    virtual void unsubscribe(EventType event, IObserver* observer) = 0;

    /**
     * @brief Notify all subscribed observers of an event.
     * @param event The event type broadcasted.
     */
    virtual void notify(EventType event) = 0;
};
