/**
 * @file Koopa.h
 * @author TV4 (Vy)
 * @brief Koopa enemy with patrol and shell-based states
 * @note Sprint 5 - walking, stationary shell, and sliding shell states
 */

#pragma once

#include "core/LevelCatalog.h"
#include "entities/Enemy.h"

#include <SFML/System/Vector2.hpp>

class Mario;

enum class KoopaState {
    WALKING,
    SHELL_IDLE,
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
    KoopaState getState() const;
    bool isKoopa() const override { return true; }
    bool isDying() const override {
        return m_isFlippedDead || isDead() || !isActive();
    }

private:
    void reverseDirection();
    bool isApproachingLedge() const;
    void rebuildShellFixture();
    void syncSpriteToFeet();

    KoopaState m_state;
    bool m_isFlippedDead = false;
    float m_patrolSpeed;

    const TileMap* m_tileMap = nullptr;

    bool m_pendingShellFixtureRebuild = false;
    Mario* m_defeatOwner = nullptr;
    int m_shellKillStreak = 0;
};
