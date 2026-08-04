/**
 * @file Goomba.cpp
 * @author TV4 (Vy)
 * @brief Implementation of Goomba patrol and stomp behaviour
 * @note Sprint 4 - basic patrol AI and wall-direction reversal
 */

#include <cmath>

#include "level/TileMap.h"
#include "entities/Goomba.h"

#include <box2d/box2d.h>
#include "core/AnimationSystem.h"

namespace {

constexpr int DEFAULT_GOOMBA_HEALTH = 1;
constexpr float DEFAULT_GOOMBA_SPEED = 60.f;

constexpr const char* GOOMBA_TEXTURE_PATH = "assets/textures/enemies/goomba.png";

const sf::Vector2f GOOMBA_SIZE{32.f, 32.f};

constexpr float TILE_SIZE = 32.f;
constexpr float EDGE_PROBE_OFFSET = 2.f;

} // namespace

Goomba::Goomba(const sf::Vector2f& position, b2World* world)
: Enemy(position, GOOMBA_SIZE, DEFAULT_GOOMBA_HEALTH),
  m_isStomped(false),
  m_patrolSpeed(DEFAULT_GOOMBA_SPEED){
      setFacingDirection(Direction::LEFT);
      initPhysics(world, b2_dynamicBody, GOOMBA_SIZE);
      setSprite(GOOMBA_TEXTURE_PATH);
      
      m_animationSystem->addAnimation("walk", AnimationSystem::createGridAnimation(0, 0, 32, 32, 2, 0.15f));
      m_animationSystem->addAnimation("squish", AnimationSystem::createGridAnimation(64, 0, 32, 32, 1, 1.f));
      playAnimation("walk");
}

void Goomba::update(float dt) {
    syncPhysics();

    if (m_position.y > 800.f) {
        markForRemoval();
        return;
    }

    if (m_isStomped) {
        // Disable the body after the Box2D physics step.
        // This lets the squish sprite remain visible without blocking Mario.
        b2Body* body = getBody();

        if (body && body->IsEnabled()) {
            body->SetEnabled(false);
        }

        m_squishTimer += dt;
        updateAnimation(dt);

        if (m_squishTimer >= SQUISH_DURATION) {
            markForRemoval();
        }

        return;
    }

    if (!isDead()) {
        patrol();
    }

    updateAnimation(dt);
}

void Goomba::onStomp() {
    if (m_isStomped) {
        return;
    }

    m_isStomped = true;
    m_squishTimer = 0.f;
    setHealth(0);
    setVelocity({0.f, 0.f});

    b2Body* body = getBody();

    if (body) {
        for (b2Fixture* fixture = body->GetFixtureList(); fixture != nullptr; fixture = fixture->GetNext()) {
            fixture->SetSensor(true);
        }
    }

    playAnimation("squish");
    updateAnimation(0.f);
}

void Goomba::patrol() {
    if (m_isStomped || isDead()) {
        return;
    }

    if (isApproachingLedge()) {
        reverseDirection();
    }

    sf::Vector2f velocity = getVelocity();

    if (getFacingDirection() == Direction::LEFT) {
        velocity.x = -m_patrolSpeed;
    } else {
        velocity.x = m_patrolSpeed;
    }

    setVelocity(velocity);
}

void Goomba::onWallCollision() {
    if (m_isStomped || isDead()) {
        return;
    }

    reverseDirection();
}

void Goomba::reverseDirection() {
    if (getFacingDirection() == Direction::LEFT) {
        setFacingDirection(Direction::RIGHT);
    } else {
        setFacingDirection(Direction::LEFT);
    }
}

bool Goomba::isStomped() const {
    return m_isStomped;
}

void Goomba::setTileMap(const TileMap* tileMap) {
    m_tileMap = tileMap;
}

bool Goomba::isApproachingLedge() const {
    if (!m_tileMap) {
        return false;
    }

    const float footY = m_position.y + m_size.y + EDGE_PROBE_OFFSET;

    const float currentX = m_position.x + m_size.x / 2.f;
    const float frontX = getFacingDirection() == Direction::LEFT
    ? m_position.x - EDGE_PROBE_OFFSET : m_position.x + m_size.x + EDGE_PROBE_OFFSET;

    const int row = static_cast<int>(std::floor(footY / TILE_SIZE));

    const int currentColumn = static_cast<int>(std::floor(currentX / TILE_SIZE));
    const int frontColumn = static_cast<int>(std::floor(frontX / TILE_SIZE));

    const bool hasCurrentGround = m_tileMap->isSolid(currentColumn, row);
    const bool hasFrontGround = m_tileMap->isSolid(frontColumn, row);

    return hasCurrentGround && !hasFrontGround;
}
