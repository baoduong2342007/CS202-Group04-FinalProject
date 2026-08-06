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
    PLAYER_DIED,
    PLAYER_POWER_UP,
    PLAYER_POWER_DOWN,
    ENEMY_STOMPED,
    COIN_COLLECTED,
    LEVEL_COMPLETED,
    GAME_PAUSED,
    LEVEL_STARTED    // S6-TV1-12: fired after level load + restore
};
