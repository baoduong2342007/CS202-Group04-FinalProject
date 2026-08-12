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
    PLAYER_STAR_COLLECTED,
    PLAYER_INVINCIBILITY_EXPIRED,
    ENEMY_STOMPED,
    ENEMY_DEFEATED_BY_SHELL,
    ENEMY_DEFEATED_BY_FIREBALL,
    ENEMY_DEFEATED_BY_STAR,
    ENEMY_DEFEATED_BY_BLOCK,
    FIREBALL_SHOT,
    SHELL_KICKED,
    BLOCK_BUMPED,
    BRICK_BROKEN,
    ITEM_EMERGED,
    ONE_UP_COLLECTED,
    COIN_COLLECTED,
    LEVEL_COMPLETED,
    GAME_PAUSED,
    GAME_RESUMED,
    LEVEL_STARTED    // S6-TV1-12: fired after level load + restore
};
