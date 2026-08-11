/**
 * @file FireBall.h
 * @author TV3 (Bảo)
 * @brief Fireball projectile entity for Mario FIRE power-up state
 * @note Week 7 implementation
 */

#pragma once

#include "entities/Entity.h"
#include "entities/Character.h" // For Direction enum

class Mario;

class FireBall : public Entity {
public:
    // 1. Constructor / Destructor
    FireBall();
    FireBall(const sf::Vector2f& position, Direction direction, b2World* world);
    ~FireBall() override = default;

    // 2. Override methods
    void update(float dt) override;

    // 3. Public methods
    void bounce(const sf::Vector2f& surfaceNormal = sf::Vector2f(0.f, -1.f));
    void deactivate(bool explode = true);
    bool shouldSpawnExplosion() const { return m_spawnExplosion; }
    void clearExplosionFlag() { m_spawnExplosion = false; }
    EntityType getType() const override { return EntityType::PROJECTILE; }
    bool isFireBall() const override { return true; }

    /// Re-initialize FireBall for Object Pooling without re-allocating memory
    void spawn(const sf::Vector2f& position, Direction direction, b2World* world);

    /// Record the Mario instance that owns this shot for score attribution.
    void setOwner(Mario* owner) { m_owner = owner; }
    Mario* getOwner() const { return m_owner; }

    // 4. Getters / Setters
    int getBounceCount() const;
    Direction getDirection() const;
    float getLifetime() const { return m_lifetime; }

private:
    // 5. Private members
    /// Assign the shared negative collision group so this fireball never
    /// physically collides with Mario (fireballs pass through the plumber).
    void applyNoPlayerCollision();

    Direction m_direction;
    int m_bounceCount;
    float m_lifetime;
    float m_bounceCooldown;
    bool m_spawnExplosion = false;
    Mario* m_owner = nullptr; // non-owning; Level owns the projectile and Mario
};
