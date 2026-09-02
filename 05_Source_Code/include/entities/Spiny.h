/**
 * @file Spiny.h
 * @brief Spiked walker hatched from Lakitu's eggs
 * @note Enemy expansion - SMB1-accurate: cannot be stomped (the spikes hurt
 *       Mario), only fireballs or a star defeat it.
 */

#pragma once

#include "core/LevelCatalog.h"
#include "entities/Enemy.h"

class TileMap;

class Spiny : public Enemy {
public:
    Spiny(const sf::Vector2f& position,
          b2World* world,
          LevelTheme theme,
          Direction initialDirection = Direction::LEFT);
    ~Spiny() override = default;

    void update(float dt) override;

    void patrol() override;
    void onStomp() override;
    void onWallCollision() override;
    void onFireHit() override;

    void setTileMap(const TileMap* tileMap) override;

    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::SPINY; }
    Capabilities getCapabilities() const noexcept override {
        return Enemy::getCapabilities() & ~capability(Capability::STOMPABLE);
    }
    bool isDying() const override {
        return m_isFlippedDead || isDead() || !isActive();
    }

private:
    void reverseDirection();
    bool isApproachingLedge() const;
    void syncSpriteToFeet();

    bool m_isFlippedDead = false;
    float m_patrolSpeed = 60.f;
    const TileMap* m_tileMap = nullptr;

    static constexpr float PIT_CLEANUP_Y = 800.f;
};
