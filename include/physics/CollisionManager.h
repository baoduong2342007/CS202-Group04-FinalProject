/**
 * @file CollisionManager.h
 * @author TV3 (Bảo)
 * @brief CollisionManager class for resolving Box2D collision pairs safely
 * @note Week 7 implementation
 */

#pragma once

#include <box2d/box2d.h>

class Entity;
class Mario;

class CollisionManager {
public:
    // ============================================================
    // PATTERN: Utility / Helper Manager
    // Reason: Decouples physical contact events from entity logic
    // ============================================================

    /// @brief Resolves beginning of collision between Box2D fixtures
    /// @param contact Pointer to Box2D contact object
    static void resolve(b2Contact* contact);

    /// @brief Adjusts active contact properties before Box2D solves them
    static void preSolve(b2Contact* contact);

private:
    CollisionManager() = delete;

    /// @brief Handles collision between Mario and another Entity safely
    static void handleMarioCollision(Mario* mario, Entity* other, b2Body* marioBody, b2Contact* contact);
};
