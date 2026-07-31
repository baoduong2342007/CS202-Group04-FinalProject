/**
 * @file Koopa.cpp
 * @author TV4 (Vy)
 * @brief Implementation of Koopa walking and patrol foundation
 * @note Sprint 5 - establishes Koopa states and horizontal patrol
 */

#include "entities/Koopa.h"

#include <box2d/box2d.h>

namespace {

constexpr int DEFAULT_KOOPA_HEALTH = 1;

constexpr float DEFAULT_KOOPA_PATROL_SPEED = 50.f;
constexpr float TILE_SIZE = 32.f;
constexpr float KOOPA_HEIGHT = 48.f;
constexpr float KOOPA_VERTICAL_SPAWN_OFFSET = KOOPA_HEIGHT - TILE_SIZE;

const sf::Vector2f KOOPA_SIZE{32.f, KOOPA_HEIGHT};

sf::Vector2f alignKoopaToGround(const sf::Vector2f& position) {
    return {position.x, position.y - KOOPA_VERTICAL_SPAWN_OFFSET};
}

} // namespace

Koopa::Koopa(const sf::Vector2f& position)
: Enemy(alignKoopaToGround(position),
        KOOPA_SIZE,
        DEFAULT_KOOPA_HEALTH
        ),
  m_state(KoopaState::WALKING),
  m_patrolSpeed(DEFAULT_KOOPA_PATROL_SPEED) {
    setFacingDirection(Direction::LEFT);
    initPhysics(b2_dynamicBody, KOOPA_SIZE);
}

void Koopa::update(float dt) {
    (void)dt;

    syncPhysics();

    if (m_state == KoopaState::WALKING && !isDead()) {
        patrol();
    }
}

void Koopa::onStomp() {
    if (m_state != KoopaState::WALKING) {
        return;
    }

    m_state = KoopaState::SHELL_IDLE;

    const sf::Vector2f currentVelocity = getVelocity();
    setVelocity({0.f, currentVelocity.y});
}

void Koopa::onWallCollision() {
    if (m_state != KoopaState::WALKING || isDead()) {
        return;
    }

    reverseDirection();
}

void Koopa::patrol() {
    if (m_state != KoopaState::WALKING || isDead()) {
        return;
    }

    sf::Vector2f velocity = getVelocity();

    if (getFacingDirection() == Direction::LEFT) {
        velocity.x = -m_patrolSpeed;
    } else {
        velocity.x = m_patrolSpeed;
    }

    setVelocity(velocity);
}

bool Koopa::isInShell() const {
    return m_state != KoopaState::WALKING;
}

KoopaState Koopa::getState() const {
    return m_state;
}

void Koopa::reverseDirection() {
    if (getFacingDirection() == Direction::LEFT) {
        setFacingDirection(Direction::RIGHT);
    } else {
        setFacingDirection(Direction::LEFT);
    }

    patrol();
}
