/**
 * @file EnemyProjectile.cpp
 * @brief EnemyProjectile base - sensor flight, lifetime, cleanup
 */

#include "entities/EnemyProjectile.h"

#include <box2d/box2d.h>

EnemyProjectile::EnemyProjectile(const sf::Vector2f& position,
                                 const sf::Vector2f& size,
                                 b2World* world,
                                 const sf::Vector2f& initialVelocity,
                                 float gravityScale)
    : Entity(position, size) {
    // Sensor body: hammers and fire pass over/through terrain exactly like
    // the NES originals, and nothing can stand on or block them.
    initPhysics(world, b2_dynamicBody, size, true);

    if (m_body) {
        m_body->SetGravityScale(gravityScale);
    }
    setVelocity(initialVelocity);
}

void EnemyProjectile::update(float dt) {
    m_lifetime += dt;
    if (m_lifetime >= MAX_LIFETIME) {
        markForRemoval();
        return;
    }

    syncPhysics();
    updateAnimation(dt);
    updatePresentation(dt);
}
