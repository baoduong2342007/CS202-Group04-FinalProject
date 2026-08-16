/**
 * @file EnemyProjectile.h
 * @brief Base class for enemy-owned projectiles (Hammer Bro's hammers,
 *        Bowser's fire breath)
 * @note These are hazards, not enemies: they cannot be stomped or shot,
 *       they only ever damage a non-star player, and star Mario walks
 *       straight through them. Movement models live in the subclasses.
 */

#pragma once

#include "entities/Entity.h"

class Mario;

class EnemyProjectile : public Entity {
public:
    EnemyProjectile(const sf::Vector2f& position,
                    const sf::Vector2f& size,
                    b2World* world,
                    const sf::Vector2f& initialVelocity,
                    float gravityScale);
    ~EnemyProjectile() override = default;

    void update(float dt) override;

    EntityType getType() const override {
        return EntityType::PROJECTILE;
    }

    // CollisionManager must never confuse these with Mario's FireBalls.
    bool isFireBall() const override { return false; }
    bool isEnemyProjectile() const override { return true; }

protected:
    /// Subclass rendering hook (rotation, wave, animation frames).
    virtual void updatePresentation(float dt) {
        (void)dt;
    }

    float m_lifetime{0.f};

    static constexpr float MAX_LIFETIME = 4.f;
};
