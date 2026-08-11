/**
 * @file CollisionManager.h
 * @author TV3 (Bảo)
 * @brief CollisionManager class for resolving Box2D collision pairs safely
 * @note Week 7 implementation
 */

#pragma once

#include <box2d/box2d.h>

#include "core/ScoreRules.h"

class TileMap;
class Entity;
class Mario;
class Enemy;

class CollisionManager {
public:
    // ============================================================
    // PATTERN: Utility / Helper Manager
    // Reason: Decouples physical contact events from entity logic
    // ============================================================

    /// @brief Resolves beginning of collision between Box2D fixtures
    /// @param contact Pointer to Box2D contact object
    static void resolve(b2Contact* contact, TileMap& tileMap);

    /// Commit one enemy defeat transaction. This is the only operation that
    /// applies defeat score and its cause-specific EventBus event.
    static bool defeatEnemy(Enemy& victim, DefeatCause cause, Mario* owner);

    /// @brief Adjusts active contact properties before Box2D solves them
    static void preSolve(b2Contact* contact, TileMap& tileMap);

private:
    CollisionManager() = delete;

    /// @brief Handles collision between Mario and another Entity safely
    static void handleMarioCollision(Mario* mario, Entity* other, b2Body* marioBody, b2Contact* contact, TileMap& tileMap);
};
