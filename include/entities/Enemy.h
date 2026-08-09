/**
 * @file Enemy.h
 * @author TV4 (Vy)
 * @brief Abstract Enemy base class for Goomba and Koopa
 * @note Sprint 4 - abstract enemy interface for patrol and collision responses
 */

#pragma once

#include "entities/Character.h"

/*
 * OOP PRINCIPLE: Runtime Polymorphism
 * Reason: Enemy defines the common interface for enemy behavior,
 *         while specific enemies such as Goomba and Koopa implement
 *         their own patrol, stomp, and wall-collision behaviour.
 */

class TileMap;

class Enemy : public Character {
public:
    Enemy(const sf::Vector2f& position, const sf::Vector2f& size, int health);

    ~Enemy() override = default;

    EntityType getType() const override {
        return EntityType::ENEMY;
    }

    void activate();
    bool isActivated() const;

    virtual void patrol() = 0;
    virtual void onStomp() = 0;
    virtual void onWallCollision() = 0;
    virtual void onFireHit();

    virtual void setTileMap(const TileMap* tileMap) {
        (void)tileMap;
    }
    virtual void onSideCollision(Entity* other);

    bool isEnemy() const override {
        return true;
    }

    virtual bool isDying() const {
        return !isActive() || isDead();
    }

    /// Claim the one shared defeat transaction for this victim.
    /// CollisionManager owns the transaction; the latch only prevents a
    /// second fixture/contact from applying damage, score, or SFX again.
    bool tryCommitDefeat();

    /// Claim the one stomp interaction for this victim. A Koopa's first stomp
    /// changes it into a shell instead of killing it, so this is separate from
    /// the terminal defeat latch.
    bool tryCommitStomp();

private:
    bool m_activated = false;
    bool m_defeatCommitted = false;
    bool m_stompCommitted = false;
};
