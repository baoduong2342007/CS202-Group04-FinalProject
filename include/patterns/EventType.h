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
    PLAYER_LOST_LIFE,     // S6: Lost a life but has lives remaining
    PLAYER_POWER_UP,
    PLAYER_POWER_DOWN,
    PLAYER_STAR_COLLECTED,
    PLAYER_INVINCIBILITY_EXPIRED,
    ENEMY_STOMPED,
    FIREBALL_SHOT,
    SHELL_KICKED,
    BLOCK_BUMPED,
    BRICK_BROKEN,
    ITEM_EMERGED,
    ONE_UP_COLLECTED,
    COIN_COLLECTED,
    LEVEL_COMPLETED,
    GAME_PAUSED,
    LEVEL_STARTED    // S6-TV1-12: fired after level load + restore
};
