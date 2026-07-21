/**
 * @file EventType.h
 * @author TV1
 * @brief Core event type enumeration for decoupled EventBus messaging
 * @note Week 4 expanded with core gameplay events
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
    PLAYER_POWER_DOWN,
    ENEMY_STOMPED,
    ENEMY_KILLED,
    COIN_COLLECTED,
    POWERUP_SPAWNED,
    FIREBALL_FIRED,
    LEVEL_COMPLETED,
    GAME_OVER,
    GAME_PAUSED
};
