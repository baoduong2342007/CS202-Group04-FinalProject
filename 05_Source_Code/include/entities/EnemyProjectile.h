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

    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::ENEMY_PROJECTILE; }
    Capabilities getCapabilities() const noexcept override {
        return capability(Capability::ENEMY_PROJECTILE) | capability(Capability::TRIGGER);
    }

protected:
    /// Subclass rendering hook (rotation, wave, animation frames).
    virtual void updatePresentation(float dt) {
        (void)dt;
    }

    float m_lifetime{0.f};

    static constexpr float MAX_LIFETIME = 4.f;
};
