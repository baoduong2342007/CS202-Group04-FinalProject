/**
 * @file BulletBill.h
 * @brief Cannon-fired bullet enemy that flies in a straight line
 * @note Enemy expansion - SMB1-accurate: stompable for 200 points,
 *       fireball-proof, star kills it, never deflects from its path.
 */

#pragma once

#include "core/LevelCatalog.h"
#include "entities/Enemy.h"

class BulletBill : public Enemy {
public:
    BulletBill(const sf::Vector2f& position,
               b2World* world,
               LevelTheme theme,
               Direction direction);
    ~BulletBill() override = default;

    void update(float dt) override;

    void patrol() override;
    void onStomp() override;
    void onWallCollision() override;
    void onFireHit() override;

    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::BULLET_BILL; }
    Capabilities getCapabilities() const noexcept override {
        return Enemy::getCapabilities() | capability(Capability::FIREPROOF);
    }
    int getStompScore() const override { return 200; }
    bool isDying() const override {
        return m_isFlippedDead || isDead() || !isActive();
    }

    Direction getDirection() const { return m_direction; }

private:
    /// Shared flipped-and-falling death presentation (stomp or star).
    void dieFlipped();

    Direction m_direction;
    bool m_isFlippedDead = false;

    static constexpr float SPEED = 240.f;
    static constexpr float PIT_CLEANUP_Y = 800.f;
};
