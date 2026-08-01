/**
 * @file Mario.cpp
 * @author TV3
 * @brief Mario character implementation
 */

#include "entities/Mario.h"
#include "patterns/EventBus.h"
#include "physics/PhysicsEngine.h"
#include "core/AnimationSystem.h"

namespace {
constexpr int DEFAULT_MARIO_HEALTH = 100;
constexpr float DEFAULT_JUMP_FORCE = 450.f;
constexpr float DEFAULT_MOVE_SPEED = 200.f;
constexpr int FATAL_DAMAGE = 100;
} // namespace

Mario::Mario()
    : Character(sf::Vector2f(100.f, 100.f), sf::Vector2f(32.f, 32.f),
                DEFAULT_MARIO_HEALTH),
      m_marioState(MarioState::SMALL), m_jumpForce(DEFAULT_JUMP_FORCE),
      m_moveSpeed(DEFAULT_MOVE_SPEED), m_score(0) {
    m_animationSystem->addAnimation("idle", AnimationSystem::createGridAnimation(0, 0, 32, 32, 1, 1.f));
    m_animationSystem->addAnimation("walk", AnimationSystem::createGridAnimation(0, 0, 32, 32, 3, 0.1f));
    m_animationSystem->addAnimation("jump", AnimationSystem::createGridAnimation(96, 0, 32, 32, 1, 1.f));
    playAnimation("idle");
}

Mario::Mario(const sf::Vector2f &position, const sf::Vector2f &size)
    : Character(position, size, DEFAULT_MARIO_HEALTH),
      m_marioState(MarioState::SMALL), m_jumpForce(DEFAULT_JUMP_FORCE),
      m_moveSpeed(DEFAULT_MOVE_SPEED), m_score(0) {
    m_animationSystem->addAnimation("idle", AnimationSystem::createGridAnimation(0, 0, 32, 32, 1, 1.f));
    m_animationSystem->addAnimation("walk", AnimationSystem::createGridAnimation(0, 0, 32, 32, 3, 0.1f));
    m_animationSystem->addAnimation("jump", AnimationSystem::createGridAnimation(96, 0, 32, 32, 1, 1.f));
    playAnimation("idle");
}

void Mario::update(float dt) {
  // Sync the entity position and velocity with the Box2D body
  syncPhysics();
  
  // Basic animation state machine
  if (!isGrounded()) {
      playAnimation("jump");
  } else if (std::abs(getVelocity().x) > 5.f) {
      playAnimation("walk");
  } else {
      playAnimation("idle");
  }

  updateAnimation(dt);
}

// DEPRECATED: Replaced by InputHandler (Command Pattern) in Game::update().
// Kept as fallback for debugging. Remove after team confirms InputHandler works.
void Mario::handleInput() {
  if (!m_body)
    return;

  b2Vec2 velocity = m_body->GetLinearVelocity();
  float desiredXVelocity = 0.0f;

  // Horizontal Movement
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
    desiredXVelocity = -PhysicsEngine::pixelsToMeters(m_moveSpeed);
    setFacingDirection(Direction::LEFT);
  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
             sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
    desiredXVelocity = PhysicsEngine::pixelsToMeters(m_moveSpeed);
    setFacingDirection(Direction::RIGHT);
  }

  // Set horizontal velocity, maintaining vertical velocity
  m_body->SetLinearVelocity(b2Vec2(desiredXVelocity, velocity.y));

  // Jump (W or Space) - Can only jump if grounded
  if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
       sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) ||
       sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) &&
      isGrounded()) {
    float jumpVelocity = -PhysicsEngine::pixelsToMeters(
        m_jumpForce); // negative Y is UP in SFML coordinates
    m_body->SetLinearVelocity(
        b2Vec2(m_body->GetLinearVelocity().x, jumpVelocity));
    setGrounded(false); // No longer grounded after jumping
  }
}

void Mario::jump() {
  if (!m_body || !isGrounded())
    return;

  float jumpVelocity = -PhysicsEngine::pixelsToMeters(m_jumpForce);
  m_body->SetLinearVelocity(
      b2Vec2(m_body->GetLinearVelocity().x, jumpVelocity));
  setGrounded(false);

  EventBus::getInstance().notify(EventType::PLAYER_JUMPED);
}

void Mario::moveLeft() {
  if (!m_body)
    return;

  float desiredXVelocity = -PhysicsEngine::pixelsToMeters(m_moveSpeed);
  m_body->SetLinearVelocity(
      b2Vec2(desiredXVelocity, m_body->GetLinearVelocity().y));
  setFacingDirection(Direction::LEFT);
}

void Mario::moveRight() {
  if (!m_body)
    return;

  float desiredXVelocity = PhysicsEngine::pixelsToMeters(m_moveSpeed);
  m_body->SetLinearVelocity(
      b2Vec2(desiredXVelocity, m_body->GetLinearVelocity().y));
  setFacingDirection(Direction::RIGHT);
}

void Mario::stopMoving() {
  if (!m_body)
    return;

  m_body->SetLinearVelocity(
      b2Vec2(0.0f, m_body->GetLinearVelocity().y));
}

void Mario::powerUp(MarioState state) {
  m_marioState = state;
  // TV3 can adjust bounding box size here for big/fire Mario if needed
}

void Mario::powerDown() {
  if (m_marioState == MarioState::FIRE) {
    m_marioState = MarioState::SUPER;
    EventBus::getInstance().notify(EventType::PLAYER_POWER_DOWN);
  } else if (m_marioState == MarioState::SUPER) {
    m_marioState = MarioState::SMALL;
    EventBus::getInstance().notify(EventType::PLAYER_POWER_DOWN);
  } else {
    if (getHealth() > 0) {
      takeDamage(FATAL_DAMAGE); // Small Mario dies
      EventBus::getInstance().notify(EventType::PLAYER_DIED);
    }
  }
}

MarioState Mario::getMarioState() const { return m_marioState; }

void Mario::setMarioState(MarioState state) { m_marioState = state; }

void Mario::addScore(int points) {
  m_score += points;
}

int Mario::getScore() const {
  return m_score;
}
