/**
 * @file Mario.cpp
 * @author TV3
 * @brief Mario character implementation
 */

#include "entities/Mario.h"
#include <iostream>
#include "physics/PhysicsEngine.h"

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
      m_moveSpeed(DEFAULT_MOVE_SPEED) {}

Mario::Mario(const sf::Vector2f &position, const sf::Vector2f &size)
    : Character(position, size, DEFAULT_MARIO_HEALTH),
      m_marioState(MarioState::SMALL), m_jumpForce(DEFAULT_JUMP_FORCE),
      m_moveSpeed(DEFAULT_MOVE_SPEED) {}

void Mario::update(float dt) {
  (void)dt;

  // Sync the entity position and velocity with the Box2D body
  syncPhysics();
}

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

void Mario::powerUp(MarioState state) {
  m_marioState = state;
  // TV3 can adjust bounding box size here for big/fire Mario if needed
}

void Mario::powerDown() {
  if (m_marioState == MarioState::FIRE) {
    m_marioState = MarioState::SUPER;
  } else if (m_marioState == MarioState::SUPER) {
    m_marioState = MarioState::SMALL;
  } else {
    takeDamage(FATAL_DAMAGE); // Small Mario dies
  }
}

MarioState Mario::getMarioState() const { return m_marioState; }

void Mario::setMarioState(MarioState state) { m_marioState = state; }
