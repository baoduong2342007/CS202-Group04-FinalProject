/**
 * @file Goomba.cpp
 * @author TV4 (Vy)
 * @brief Implementation of Goomba patrol and stomp behaviour
 * @note Sprint 4 - basic patrol AI and wall-direction reversal
 */

#include "entities/Goomba.h"

#include <box2d/box2d.h>

namespace {

constexpr int DEFAULT_GOOMBA_HEALTH = 1;
constexpr float DEFAULT_GOOMBA_SPEED = 60.f;

constexpr const char* GOOMBA_TEXTURE_PATH = "assets/textures/enemies/goomba.png";

const sf::Vector2f GOOMBA_SIZE{32.f, 32.f};

} // namespace

Goomba::Goomba(const sf::Vector2f& position)
: Enemy(position, GOOMBA_SIZE, DEFAULT_GOOMBA_HEALTH),
  m_isStomped(false),
  m_patrolSpeed(DEFAULT_GOOMBA_SPEED){
      setFacingDirection(Direction::LEFT);
      initPhysics(b2_dynamicBody, GOOMBA_SIZE);
      setSprite(GOOMBA_TEXTURE_PATH);
}

void Goomba::update(float dt){
    // Patrol speed is constant, so dt is not directly needed here.
    (void)dt;

    if (!m_isStomped && !isDead()){
        patrol();
    }

    syncPhysics();
}

void Goomba::onStomp(){
    if (m_isStomped){
        return;
    }

    m_isStomped = true;
    setHealth(0);

    const sf::Vector2f currentVelocity = getVelocity();
    setVelocity({0.f, currentVelocity.y});
}

void Goomba::patrol(){
    if (m_isStomped || isDead()){
        return;
    }

    sf::Vector2f velocity = getVelocity();

    if (getFacingDirection() == Direction::LEFT){
        velocity.x = -m_patrolSpeed;
    } else {
        velocity.x = m_patrolSpeed;
    }

    setVelocity(velocity);
}

void Goomba::onWallCollision(){
    if (m_isStomped || isDead()){
        return;
    }

    reverseDirection();
}

void Goomba::reverseDirection(){
    if (getFacingDirection() == Direction::LEFT){
        setFacingDirection(Direction::RIGHT);
    } else {
        setFacingDirection(Direction::LEFT);
    }

    patrol();
}

bool Goomba::isStomped() const {
    return m_isStomped;
}
