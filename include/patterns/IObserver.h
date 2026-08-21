/**
 * @file IObserver.h
 * @author TV1
 * @brief Observer interface for receiving notifications from EventBus
 * @note Week 2
 */

#pragma once

#include "patterns/GameEvent.h"

/**
 * @brief Interface for any object that wants to listen to game events.
 * @note EventBus subscribers should retain the move-only Subscription returned
 *       by subscribe(); its lifetime controls the registration.
 */
class IObserver {
public:
    virtual ~IObserver() = default;

    /**
     * @brief Called by EventSubject/EventBus when a subscribed event occurs.
     * @param event The value event that occurred.
     */
    virtual void onNotify(const GameEvent& event) = 0;
};
