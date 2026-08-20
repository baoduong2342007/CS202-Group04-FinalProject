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
    /// Star-contact response; defaults to the fireball presentation so
    /// existing enemies keep their flipped death. Bowser overrides this to
    /// die outright to star power.
    virtual void onStarHit();

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

    /// Whether Mario's fireballs simply burst against this enemy without
    /// defeating it (Buzzy Beetle shell, Bullet Bill).
    virtual bool isFireproof() const {
        return false;
    }

    /// Whether a top contact counts as a stomp. Swimming Cheep Cheeps,
    /// Bloopers, and Podoboo harm Mario instead of being stompable.
    virtual bool canBeStomped() const {
        return true;
    }

    /// Whether this enemy is absolutely unkillable by any defeat cause
    /// (the Podoboo lava bubble ignores star, fire, and shell alike).
    virtual bool isIndestructible() const {
        return false;
    }

    /// Species-specific stomp score; 0 = use the shared airborne stomp
    /// chain (Bullet Bill 200, Lakitu 800, Hammer Bro 1000).
    virtual int getStompScore() const {
        return 0;
    }

    /// Species-specific score for a non-stomp defeat; 0 = use the cause
    /// default. The cause value matches CollisionManager's DefeatCause.
    virtual int getDefeatScore(int cause) const {
        (void)cause;
        return 0;
    }

    /// Hits a fireball must land before the enemy falls (multi-hit boss).
    /// 0 = the classic one-shot transaction. Bowser returns 5.
    virtual int getFireballHealth() const {
        return 0;
    }

    /// Updates nearest player position for breakout AI and player awareness.
    virtual void updatePlayerPosition(const sf::Vector2f& playerPos);

    /// Whether this enemy is currently breaking out of a narrow 1-2 tile patrol pocket towards the player.
    bool isEscapingNarrowRange() const;

    /// Called when the enemy turns around (wall collision or ledge) to detect oscillation in 1-2 tile range.
    void notifyTurnaround();

    /// Updates narrow escape status and resets to normal logic once clear of the trapped bounds.
    void updateNarrowEscapeStatus();

    /// Claim the one shared defeat transaction for this victim.
    /// CollisionManager owns the transaction; the latch only prevents a
    /// second fixture/contact from applying damage, score, or SFX again.
    bool tryCommitDefeat();

    /// Claim the one stomp interaction for this victim. A Koopa's first stomp
    /// changes it into a shell instead of killing it, so this is separate from
    /// the terminal defeat latch.
    bool tryCommitStomp();

protected:
    /// Re-arm the one-shot stomp latch. Paratroopa uses this after a stomp
    /// only clipped its wings, so a second stomp can still claim the shell
    /// transition.
    void allowNextStomp();

    sf::Vector2f m_playerPos{0.f, 0.f};
    bool m_hasPlayerPos = false;

private:
    bool m_activated = false;
    bool m_defeatCommitted = false;
    bool m_stompCommitted = false;

    float m_lastTurnX = -99999.f;
    int m_narrowTurnCount = 0;
    bool m_isEscapingNarrowRange = false;
    float m_trappedMinX = 0.f;
    float m_trappedMaxX = 0.f;
    float m_trappedY = 0.f;
};
