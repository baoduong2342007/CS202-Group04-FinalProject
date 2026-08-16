/**
 * @file CollisionManager.h
 * @author TV3 (Bảo)
 * @brief CollisionManager class for resolving Box2D collision pairs safely
 * @note Week 7 implementation
 */

#pragma once

#include <box2d/box2d.h>

#include <vector>

#include "core/ScoreRules.h"

#include <SFML/System/Vector2.hpp>

class TileMap;
class Entity;
class Mario;
class Enemy;

/// One deferred player-vs-player contact outcome, produced inside the Box2D
/// step and applied by Level after the step completes.
struct PvpHit {
    enum class Cause {
        STOMP,     ///< attacker landed on victim's head — round-winning stomp
        FIREBALL   ///< attacker's fireball struck victim — knockback + stun
    };

    Cause cause;
    Mario* attacker = nullptr;
    Mario* victim = nullptr;
    sf::Vector2f position{0.f, 0.f};
};

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
    static bool defeatEnemy(Enemy& victim, DefeatCause cause, Mario* owner, int streakIndex = 0);

    /// @brief Adjusts active contact properties before Box2D solves them
    static void preSolve(b2Contact* contact, TileMap& tileMap);

    /// Drain the deferred PvP contact queue (stomp KOs and fireball hits).
    /// The queue is emptied by the call; callers own the returned hits.
    static std::vector<PvpHit> consumePendingPvpHits();

    /// Drop any queued PvP hits, e.g. when a PvP arena is (re)loaded.
    static void clearPendingPvpHits();

private:
    CollisionManager() = delete;

    /// @brief Handles collision between Mario and another Entity safely
    static void handleMarioCollision(Mario* mario, Entity* other, b2Body* marioBody, b2Contact* contact, TileMap& tileMap);

    /// @brief Handles a contact between the two PvP fighters; queues a stomp
    ///        KO when one fighter's head-stomps the other mid-descent.
    static void handlePvpPlayerCollision(Mario* playerA, Mario* playerB, b2Contact* contact);

    static void queuePvpHit(const PvpHit& hit);

    /// Deferred PvP contacts generated inside b2World::Step callbacks.
    /// Level drains this queue after the step, mirroring the deferred stomp
    /// score awards pattern.
    static std::vector<PvpHit> s_pendingPvpHits;
};
