/**
 * @file Goomba.cpp
 * @author TV4 (Vy)
 * @brief Implementation of Goomba patrol and stomp behaviour
 * @note Sprint 4 - basic patrol AI and wall-direction reversal
 */

#include "entities/Goomba.h"

#include <memory>

#include <box2d/box2d.h>

#include "core/AnimationSystem.h"

namespace {

constexpr int DEFAULT_GOOMBA_HEALTH = 1;
constexpr float DEFAULT_GOOMBA_SPEED = 60.f;

constexpr const char* GOOMBA_TEXTURE_PATH = "assets/textures/enemies/goomba.png";

const sf::Vector2f GOOMBA_SIZE{32.f, 32.f};

constexpr int GOOMBA_FRAME_START_X = 0;
constexpr int GOOMBA_FRAME_START_Y = 0;
constexpr int GOOMBA_FRAME_WIDTH = 32;
constexpr int GOOMBA_FRAME_HEIGHT = 32;
constexpr int GOOMBA_WALK_FRAME_COUNT = 2;

constexpr float GOOMBA_WALK_FRAME_DURATION = 0.15f;

constexpr const char* GOOMBA_WALK_ANIMATION = "walk";

constexpr int GOOMBA_SQUISH_FRAME_START_X = 64;
constexpr int GOOMBA_SQUISH_FRAME_COUNT = 1;

constexpr float GOOMBA_SQUISH_DURATION = 0.5f;

constexpr const char* GOOMBA_SQUISH_ANIMATION = "squish";

} // namespace

Goomba::Goomba(const sf::Vector2f& position)
: Enemy(position, GOOMBA_SIZE, DEFAULT_GOOMBA_HEALTH),
  m_isStomped(false),
  m_patrolSpeed(DEFAULT_GOOMBA_SPEED),
  m_squishElapsedTime(0.f) {
      setFacingDirection(Direction::LEFT);
      initPhysics(b2_dynamicBody, GOOMBA_SIZE);
      setSprite(GOOMBA_TEXTURE_PATH);
      m_animationSystem = std::make_unique<AnimationSystem>();
      
      const Animation walkAnimation =
      AnimationSystem::createGridAnimation(GOOMBA_FRAME_START_X,
                                           GOOMBA_FRAME_START_Y,
                                           GOOMBA_FRAME_WIDTH,
                                           GOOMBA_FRAME_HEIGHT,
                                           GOOMBA_WALK_FRAME_COUNT,
                                           GOOMBA_WALK_FRAME_DURATION,
                                           true
                                           );
      
      const Animation squishAnimation =
      AnimationSystem::createGridAnimation(GOOMBA_SQUISH_FRAME_START_X,
                                           GOOMBA_FRAME_START_Y,
                                           GOOMBA_FRAME_WIDTH,
                                           GOOMBA_FRAME_HEIGHT,
                                           GOOMBA_SQUISH_FRAME_COUNT,
                                           GOOMBA_SQUISH_DURATION,
                                           false
                                           );
      
      m_animationSystem->addAnimation(GOOMBA_WALK_ANIMATION,
                                      walkAnimation
                                      );
      
      m_animationSystem->addAnimation(GOOMBA_SQUISH_ANIMATION,
                                      squishAnimation
                                      );
      
      playAnimation(GOOMBA_WALK_ANIMATION);
}

void Goomba::update(float dt) {
    syncPhysics();

    if (m_isStomped) {
        b2Body* body = getBody();
        if (body && body->IsEnabled()) {
            body->SetEnabled(false);
        }

        m_squishElapsedTime += dt;
        updateAnimation(dt);

        if (m_squishElapsedTime >= GOOMBA_SQUISH_DURATION) {
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
    m_squishElapsedTime = 0.f;
    setHealth(0);
    setVelocity({0.f, 0.f});

    b2Body* body = getBody();
    if (body) {
        for (b2Fixture* fixture = body->GetFixtureList();
             fixture != nullptr;
             fixture = fixture->GetNext()) {
            fixture->SetSensor(true);
        }
    }

    playAnimation(GOOMBA_SQUISH_ANIMATION);

    updateAnimation(0.f);
}

void Goomba::patrol() {
    if (m_isStomped || isDead()) {
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

    patrol();
}

bool Goomba::isStomped() const {
    return m_isStomped;
}
