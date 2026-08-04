/**
 * @file Goomba.cpp
 * @author TV4 (Vy)
 * @brief Implementation of Goomba patrol and stomp behaviour
 * @note Sprint 4 - basic patrol AI and wall-direction reversal
 */

#include "entities/Goomba.h"

#include <box2d/box2d.h>
#include "core/AnimationSystem.h"

namespace {

constexpr int DEFAULT_GOOMBA_HEALTH = 1;
constexpr float DEFAULT_GOOMBA_SPEED = 60.f;

constexpr const char* GOOMBA_TEXTURE_PATH = "assets/textures/enemies/goomba.png";

const sf::Vector2f GOOMBA_SIZE{32.f, 32.f};

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
    // TV4 Sprint 4 Fix: Sync physics FIRST to get the gravity-affected velocity
    // before patrol() modifies it. Otherwise Goomba will float in the air!
    syncPhysics();
    
    if (m_position.y > 800.f) {
        markForRemoval();
        return;
    }

    if (!m_isStomped && !isDead()) {
        patrol();
    } else if (m_isStomped) {
        m_squishTimer += dt;
        if (m_squishTimer >= SQUISH_DURATION) {
            markForRemoval();
        }
    }
    
    updateAnimation(dt);
}

void Goomba::onStomp() {
    if (m_isStomped) {
        return;
    }

    m_isStomped = true;
    setHealth(0);

    const sf::Vector2f currentVelocity = getVelocity();
    setVelocity({0.f, currentVelocity.y});
    
    playAnimation("squish");

    // TV4 Sprint 5 Fix: Use a 0.5s despawn timer in update() instead of instant removal
    // markForRemoval();
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
