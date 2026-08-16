/**
 * @file Koopa.h
 * @author TV4 (Vy)
 * @brief Koopa enemy with patrol and shell-based states
 * @note Sprint 5 - walking, stationary shell, and sliding shell states
 * @note Enemy expansion - shell wake-up cycle and protected variant surface
 *       (BuzzyBeetle / RedKoopa / Paratroopa subclass this class)
 */

#pragma once

#include "core/LevelCatalog.h"
#include "entities/Enemy.h"

#include <SFML/System/Vector2.hpp>

class Mario;

enum class KoopaState {
    WALKING,
    SHELL_IDLE,
    SHELL_WAKING,
    SHELL_SLIDING
};

class Koopa : public Enemy {
public:
    explicit Koopa(const sf::Vector2f& position,
                   b2World* world,
                   LevelTheme theme = LevelTheme::OVERWORLD);
    ~Koopa() override = default;

    void update(float dt) override;

    void onStomp() override;
    void onWallCollision() override;
    void patrol() override;
    void onFireHit() override;

    void setTileMap(const TileMap* tileMap) override;

    void kick(Direction direction);

    /// Mario who last kicked this shell; used as the score owner if the shell
    /// later defeats another enemy.
    void setDefeatOwner(Mario* owner) { m_defeatOwner = owner; }
    Mario* getDefeatOwner() const { return m_defeatOwner; }

    int getShellKillStreak() const { return m_shellKillStreak; }
    void incrementShellKillStreak() { ++m_shellKillStreak; }
    void resetShellKillStreak() { m_shellKillStreak = 0; }

    bool isInShell() const;
    bool isShellSliding() const;
    bool isShellWaking() const;
    KoopaState getState() const;
    bool isKoopa() const override { return true; }
    bool isDying() const override {
        return m_isFlippedDead || isDead() || !isActive();
    }

protected:
    /// Variant constructor: subclasses supply their own walking/shell
    /// geometry (Buzzy Beetle is 32x32, Koopa variants are 32x48).
    Koopa(const sf::Vector2f& position,
          b2World* world,
          LevelTheme theme,
          const sf::Vector2f& walkSize,
          const sf::Vector2f& shellSize,
          float patrolSpeed);

    /// Re-register the shared animation clips with variant frame rects.
    /// Subclasses call this from their constructors to swap palettes
    /// (AnimationSystem::addAnimation replaces an existing clip name).
    void registerKoopaAnimations(const std::vector<sf::IntRect>& walkFrames,
                                 const sf::IntRect& shellRect,
                                 const sf::IntRect& wakingRect);

    /// Whether the idle shell shakes a legs-out warning frame before the
    /// wake-up. Koopa shells warn; a Buzzy Beetle emerges without warning.
    virtual bool showsWakeWarning() const { return true; }

    void enterShellState();
    void wakeUpFromShell();
    void restoreWalkingFixture();
    void reverseDirection();
    bool isApproachingLedge() const;
    void rebuildShellFixture();
    void syncSpriteToFeet();

    KoopaState m_state;
    bool m_isFlippedDead = false;
    float m_patrolSpeed;

    const TileMap* m_tileMap = nullptr;

    bool m_pendingShellFixtureRebuild = false;
    bool m_pendingWalkFixtureRestore = false;
    Mario* m_defeatOwner = nullptr;
    int m_shellKillStreak = 0;

    /// Seconds the shell has rested; after SHELL_WAKE_DELAY it starts waking.
    float m_shellIdleTimer = 0.f;
    /// Seconds spent in the legs-out SHELL_WAKING warning window.
    float m_wakingTimer = 0.f;

    sf::Vector2f m_shellSize;

    static constexpr float SHELL_WAKE_DELAY = 5.f;
    static constexpr float SHELL_WAKE_DURATION = 1.5f;
};
