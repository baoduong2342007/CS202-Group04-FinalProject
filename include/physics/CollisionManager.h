/**
 * @file CollisionManager.h
 * @author TV3 (Bao)
 * @brief CollisionManager class for resolving Box2D collision pairs safely
 * @note Week 7 implementation
 */

#pragma once

#include <box2d/box2d.h>

#include <vector>

#include "core/ScoreRules.h"
#include "entities/Entity.h"

#include <SFML/System/Vector2.hpp>

class TileMap;
class Entity;
class Mario;
class Enemy;
class FireBall;
class Item;
class Koopa;
class Mushroom;
class Star;
class Springboard;

/// Non-owning, checked view of one Box2D collision participant. Typed access
/// methods validate the entity's declared contract and concrete base before
/// policy code receives a pointer, so malformed identities fail closed.
class CollisionParticipant {
public:
    CollisionParticipant() = default;
    CollisionParticipant(Entity* entity, b2Body* body) noexcept;

    Entity* entity() const noexcept { return m_entity; }
    b2Body* body() const noexcept { return m_body; }
    Entity::EntityType type() const noexcept;
    Entity::EntitySubtype subtype() const noexcept;
    bool has(Entity::Capability capability) const noexcept;

    Mario* mario() const noexcept;
    Enemy* enemy() const noexcept;
    FireBall* fireBall() const noexcept;
    Item* item() const noexcept;
    Koopa* shell() const noexcept;
    Mushroom* mushroom() const noexcept;
    Star* star() const noexcept;
    Springboard* springboard() const noexcept;

private:
    Entity* m_entity = nullptr;
    b2Body* m_body = nullptr;
};

/// Typed two-entity collision transaction. `normal` points from first to
/// second; `normalFrom(p)` preserves that convention independently of Box2D
/// fixture order.
class CollisionContext {
public:
    CollisionContext(CollisionParticipant first,
                     CollisionParticipant second,
                     b2Contact* contact,
                     const b2Vec2& normal) noexcept;

    const CollisionParticipant& first() const noexcept { return m_first; }
    const CollisionParticipant& second() const noexcept { return m_second; }
    b2Contact* contact() const noexcept { return m_contact; }
    b2Vec2 normalFrom(const CollisionParticipant& participant) const noexcept;
    const CollisionParticipant* other(const CollisionParticipant& participant) const noexcept;

private:
    CollisionParticipant m_first;
    CollisionParticipant m_second;
    b2Contact* m_contact = nullptr;
    b2Vec2 m_normal{0.f, 0.f};
};

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

    /// Genuine two-participant gameplay dispatch with a typed context.
    static void dispatch(CollisionContext& context, TileMap& tileMap);

    /// Completes the entity callback lifecycle for a Box2D EndContact.
    static void end(b2Contact* contact);

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
