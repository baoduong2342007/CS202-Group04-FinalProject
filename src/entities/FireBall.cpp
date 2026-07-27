/**
 * @file FireBall.cpp
 * @author TV3
 * @brief Fireball entity projectile implementation with Box2D physics scaling
 */

#include "entities/FireBall.h"
#include <iostream>
#include "physics/PhysicsEngine.h"

namespace {
const sf::Vector2f FIREBALL_SIZE(12.f, 12.f);
constexpr float FIREBALL_SPEED = 350.f;
constexpr float FIREBALL_BOUNCE_SPEED = 200.f;
constexpr int MAX_BOUNCES = 3;
constexpr float MAX_LIFETIME = 3.0f;
} // namespace

FireBall::FireBall()
    : Entity({0.f, 0.f}, FIREBALL_SIZE),
      m_direction(Direction::RIGHT),
      m_bounceCount(0),
      m_lifetime(0.f) {
    initPhysics(b2_dynamicBody, FIREBALL_SIZE, false);
}

FireBall::FireBall(const sf::Vector2f& position, Direction direction)
    : Entity(position, FIREBALL_SIZE),
      m_direction(direction),
      m_bounceCount(0),
      m_lifetime(0.f) {
    initPhysics(b2_dynamicBody, FIREBALL_SIZE, false);

    if (m_body) {
        float dirMultiplier = (direction == Direction::RIGHT) ? 1.0f : -1.0f;
        float vxMeters = PhysicsEngine::pixelsToMeters(FIREBALL_SPEED * dirMultiplier);
        float vyMeters = PhysicsEngine::pixelsToMeters(FIREBALL_BOUNCE_SPEED);
        m_body->SetLinearVelocity(b2Vec2(vxMeters, vyMeters));
    }
}

void FireBall::update(float dt) {
    if (!m_active) return;

    m_lifetime += dt;
    if (m_lifetime >= MAX_LIFETIME || m_bounceCount >= MAX_BOUNCES) {
        deactivate();
        return;
    }

    if (m_body) {
        b2Vec2 vel = m_body->GetLinearVelocity();
        float dirMultiplier = (m_direction == Direction::RIGHT) ? 1.0f : -1.0f;
        float targetVx = PhysicsEngine::pixelsToMeters(FIREBALL_SPEED * dirMultiplier);

        // Maintain constant horizontal velocity while allowing Box2D gravity to drive Y velocity
        m_body->SetLinearVelocity(b2Vec2(targetVx, vel.y));
    }

    syncPhysics();
}

void FireBall::bounce() {
    if (!m_body) return;

    m_bounceCount++;
    if (m_bounceCount >= MAX_BOUNCES) {
        deactivate();
        return;
    }

    b2Vec2 vel = m_body->GetLinearVelocity();
    float bounceVy = -PhysicsEngine::pixelsToMeters(FIREBALL_BOUNCE_SPEED);
    m_body->SetLinearVelocity(b2Vec2(vel.x, bounceVy));
}

void FireBall::deactivate() {
    m_active = false;
    markForDestroy();
    destroyPhysicsBody();
}

int FireBall::getBounceCount() const {
    return m_bounceCount;
}

Direction FireBall::getDirection() const {
    return m_direction;
}
