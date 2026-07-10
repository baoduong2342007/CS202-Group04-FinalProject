/**
 * @file EventType.h
 * @author TV1
 * @brief Core event type enumeration for decoupled EventBus messaging
 * @note Week 2
 */

#pragma once

/**
 * @brief Represents global game event types broadcasted via EventBus.
 */
enum class EventType {
    PLAYER_JUMPED,
    PLAYER_LANDED,
    PLAYER_DIED,
    PLAYER_POWER_UP,
    ENEMY_KILLED,
    COIN_COLLECTED,
    LEVEL_COMPLETED
};
