/**
 * @file Koopa.h
 * @author TV4 (Vy)
 * @brief Koopa enemy with patrol and shell-based states
 * @note Sprint 5 - walking, stationary shell, and sliding shell states
 */

#pragma once

#include "entities/Enemy.h"

#include <SFML/System/Vector2.hpp>

enum class KoopaState {
    WALKING,
    SHELL_IDLE,
    SHELL_SLIDING
};

class Koopa : public Enemy {
public:
    explicit Koopa(const sf::Vector2f& position, b2World* world);
    ~Koopa() override = default;

    void update(float dt) override;

    void onStomp() override;
    void onWallCollision() override;
    void patrol() override;
    void onFireHit() override;

    void setTileMap(const TileMap* tileMap) override;

    void kick(Direction direction);

    bool isInShell() const;
    bool isShellSliding() const;
    KoopaState getState() const;
    bool isKoopa() const override { return true; }

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
};
