/**
 * @file FireBall.cpp
 * @author TV3
 * @brief Fireball entity projectile implementation with safe deferred Box2D body destruction
 */

#include "entities/FireBall.h"
#include <iostream>
#include "physics/PhysicsEngine.h"

namespace {
const sf::Vector2f FIREBALL_SIZE(12.f, 12.f);
constexpr float FIREBALL_SPEED = 360.f;
constexpr float FIREBALL_BOUNCE_SPEED = 240.f;
constexpr int MAX_BOUNCES = 3;
constexpr float MAX_LIFETIME = 3.0f;
constexpr float MIN_BOUNCE_COOLDOWN = 0.15f;
} // namespace

FireBall::FireBall()
    : Entity({0.f, 0.f}, FIREBALL_SIZE),
      m_direction(Direction::RIGHT),
      m_bounceCount(0),
      m_lifetime(0.f),
      m_bounceCooldown(0.f) {
    initPhysics(b2_dynamicBody, FIREBALL_SIZE, false);
}

FireBall::FireBall(const sf::Vector2f& position, Direction direction)
    : Entity(position, FIREBALL_SIZE),
      m_direction(direction),
      m_bounceCount(0),
      m_lifetime(0.f),
      m_bounceCooldown(0.f) {
    initPhysics(b2_dynamicBody, FIREBALL_SIZE, false);

    if (m_body) {
        float dirMultiplier = (direction == Direction::RIGHT) ? 1.0f : -1.0f;
        float vxMeters = PhysicsEngine::pixelsToMeters(FIREBALL_SPEED * dirMultiplier);
        float vyMeters = PhysicsEngine::pixelsToMeters(FIREBALL_BOUNCE_SPEED * 0.5f);
        m_body->SetLinearVelocity(b2Vec2(vxMeters, vyMeters));
    }
}

void FireBall::update(float dt) {
    if (!m_active || m_pendingDestroy) return;

    m_lifetime += dt;
    if (m_bounceCooldown > 0.0f) {
        m_bounceCooldown -= dt;
    }

    if (m_lifetime >= MAX_LIFETIME || m_bounceCount >= MAX_BOUNCES) {
        deactivate();
        return;
    }

    if (m_body) {
        b2Vec2 vel = m_body->GetLinearVelocity();
        float dirMultiplier = (m_direction == Direction::RIGHT) ? 1.0f : -1.0f;
        float targetVx = PhysicsEngine::pixelsToMeters(FIREBALL_SPEED * dirMultiplier);

        // Maintain constant horizontal speed while allowing Box2D gravity to drive Y motion
        m_body->SetLinearVelocity(b2Vec2(targetVx, vel.y));
    }

    syncPhysics();
}

void FireBall::bounce(const sf::Vector2f& surfaceNormal) {
    (void)surfaceNormal;

    if (!m_body || !m_active || m_pendingDestroy || m_bounceCooldown > 0.0f) return;

    m_bounceCount++;
    m_bounceCooldown = MIN_BOUNCE_COOLDOWN;

    if (m_bounceCount >= MAX_BOUNCES) {
        deactivate();
        return;
    }

    b2Vec2 vel = m_body->GetLinearVelocity();
    float dirMultiplier = (m_direction == Direction::RIGHT) ? 1.0f : -1.0f;
    float targetVx = PhysicsEngine::pixelsToMeters(FIREBALL_SPEED * dirMultiplier);
    float bounceVy = -PhysicsEngine::pixelsToMeters(FIREBALL_BOUNCE_SPEED);

    // Apply upward bounce velocity and maintain horizontal travel speed
    m_body->SetLinearVelocity(b2Vec2(targetVx, bounceVy));

#ifdef DEBUG
    std::cout << "[DEBUG][FireBall] Bounced cleanly! Bounce count: " << m_bounceCount << std::endl;
#endif
}

void FireBall::deactivate() {
    if (!m_active && m_pendingDestroy) return;

    m_active = false;
    markForDestroy();
    // CRITICAL: Do NOT call destroyPhysicsBody() synchronously here!
    // Calling world->DestroyBody() inside a Box2D step callback crashes Box2D.
    // Body destruction occurs safely outside world->Step() when ~Entity() runs.
}

int FireBall::getBounceCount() const {
    return m_bounceCount;
}

Direction FireBall::getDirection() const {
    return m_direction;
}
