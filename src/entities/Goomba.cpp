/**
 * @file Goomba.cpp
 * @author TV4 (Vy)
 * @brief Goomba enemy with patrol, ledge detection, and stomp behaviour
 * @note Sprint 5 - adds ledge detection and delayed removal after stomp
 */

#include <cmath>

#include "level/TileMap.h"
#include "entities/Goomba.h"

#include <box2d/box2d.h>
#include "core/AnimationSystem.h"
#include "core/SpriteFrames.h"
#include "core/SoundManager.h"

namespace {

constexpr int DEFAULT_GOOMBA_HEALTH = 1;
constexpr float DEFAULT_GOOMBA_SPEED = 60.f;
constexpr float PIT_CLEANUP_Y = 800.f;

constexpr const char* GOOMBA_TEXTURE_PATH = "assets/textures/enemies/enemies.png";

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
      
      m_animationSystem->addAnimation("walk", AnimationSystem::createManualAnimation(SpriteFrames::Enemies::Goomba::walkFrames(), 0.15f));
      m_animationSystem->addAnimation("squish", AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{SpriteFrames::Enemies::Goomba::STOMPED}, 1.f, false));
      playAnimation("walk");
}

void Goomba::update(float dt) {
    if (m_isFlippedDead) {
        syncPhysics();
        if (m_sprite) {
            m_sprite->setPosition(m_position + sf::Vector2f(m_size.x / 2.f, m_size.y / 2.f));
            m_sprite->setOrigin({8.f, 8.f});
            m_sprite->setScale({2.f, -2.f}); // Upside down flip!
        }
        if (m_position.y > PIT_CLEANUP_Y) {
            markForRemoval();
        }
        return;
    }

    if (m_isStomped) {
        b2Body* body = getBody();
        if (body && body->IsEnabled()) {
            body->SetEnabled(false);
        }

        m_squishTimer += dt;
        updateAnimation(dt);
        if (m_sprite) {
            m_sprite->setPosition(m_position);
            m_sprite->setScale({2.f, 2.f});
        }

        if (m_squishTimer >= SQUISH_DURATION) {
            markForRemoval();
        }

        return;
    }

    syncPhysics();

    if (m_position.y > PIT_CLEANUP_Y) {
        markForRemoval();
        return;
    }

    if (!isDead()) {
        patrol();
    }

    updateAnimation(dt);
}

void Goomba::onFireHit() {
    if (m_isFlippedDead || m_isStomped) return;

    m_isFlippedDead = true;
    setHealth(0);
    SoundManager::getInstance().playSound("kickkill");

    b2Body* body = getBody();
    if (body) {
        for (b2Fixture* fixture = body->GetFixtureList(); fixture != nullptr; fixture = fixture->GetNext()) {
            fixture->SetSensor(true);
        }
        body->SetLinearVelocity(b2Vec2(0.f, -8.f));
    }
}

void Goomba::onStomp() {
    if (m_isStomped) {
        return;
    }

    float groundY = m_position.y + m_size.y;

    m_isStomped = true;
    m_squishTimer = 0.f;
    setHealth(0);
    setVelocity({0.f, 0.f});

    m_size = {32.f, 16.f};
    m_position.y = groundY - 16.f; // Align bottom edge of 16px height squish sprite to ground

    b2Body* body = getBody();

    if (body) {
        for (b2Fixture* fixture = body->GetFixtureList(); fixture != nullptr; fixture = fixture->GetNext()) {
            fixture->SetSensor(true);
        }
    }

    playAnimation("squish");
    updateAnimation(0.f);
    if (m_sprite) {
        m_sprite->setPosition(m_position);
        m_sprite->setScale({2.f, 2.f});
    }
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
