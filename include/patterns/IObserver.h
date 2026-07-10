/**
 * @file IObserver.h
 * @author TV1
 * @brief Observer interface for receiving notifications from EventBus
 * @note Week 2
 */

#pragma once

#include "patterns/EventType.h"

/**
 * @brief Interface for any object that wants to listen to game events.
 * @note Observers MUST unsubscribe from EventBus in their destructor to prevent dangling pointers.
 */
class IObserver {
public:
    virtual ~IObserver() = default;

    /**
     * @brief Called by EventSubject/EventBus when a subscribed event occurs.
     * @param event The type of event that occurred.
     */
    virtual void onNotify(EventType event) = 0;
};
