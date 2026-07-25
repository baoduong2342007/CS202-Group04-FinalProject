/**
 * @file Mario.cpp
 * @author TV3
 * @brief Mario character implementation
 */

#include "entities/Mario.h"
#include <iostream>
#include "physics/PhysicsEngine.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"

namespace {
constexpr int DEFAULT_MARIO_HEALTH = 100;
constexpr float DEFAULT_JUMP_FORCE = 450.f;
constexpr float DEFAULT_MOVE_SPEED = 200.f;
constexpr float MAX_FALL_SPEED = 600.f;
constexpr int FATAL_DAMAGE = 100;

// Dimensions & Physics Constants
const sf::Vector2f DEFAULT_MARIO_POSITION(100.f, 100.f);
const sf::Vector2f SMALL_MARIO_SIZE(32.f, 32.f);
const sf::Vector2f SUPER_MARIO_SIZE(32.f, 64.f);
constexpr float MARIO_FIXTURE_DENSITY = 1.0f;
constexpr float MARIO_FIXTURE_FRICTION = 0.0f; // Zero friction to prevent wall sticking
} // namespace

Mario::Mario()
    : Character(DEFAULT_MARIO_POSITION, SMALL_MARIO_SIZE, DEFAULT_MARIO_HEALTH),
      m_marioState(MarioState::SMALL), m_jumpForce(DEFAULT_JUMP_FORCE),
      m_moveSpeed(DEFAULT_MOVE_SPEED) {}

Mario::Mario(const sf::Vector2f &position, const sf::Vector2f &size)
    : Character(position, size, DEFAULT_MARIO_HEALTH),
      m_marioState(MarioState::SMALL), m_jumpForce(DEFAULT_JUMP_FORCE),
      m_moveSpeed(DEFAULT_MOVE_SPEED) {}

void Mario::update(float dt) {
  (void)dt;

  // Clamp terminal fall velocity to prevent AABB tunneling through floor tiles
  if (m_body) {
    b2Vec2 velocity = m_body->GetLinearVelocity();
    float maxFallMeters = PhysicsEngine::pixelsToMeters(MAX_FALL_SPEED);
    if (velocity.y > maxFallMeters) {
      m_body->SetLinearVelocity(b2Vec2(velocity.x, maxFallMeters));
    }
  }

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

  // Jump (W, Space, or Up) - Can only jump if grounded
  if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
       sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) ||
       sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) &&
      isGrounded()) {
    float jumpVelocity = -PhysicsEngine::pixelsToMeters(
        m_jumpForce); // negative Y is UP in SFML coordinates
    m_body->SetLinearVelocity(
        b2Vec2(m_body->GetLinearVelocity().x, jumpVelocity));
    setGrounded(false); // No longer grounded after jumping

#ifdef DEBUG
    std::cout << "[DEBUG][Mario] Jump executed with velocity: " << jumpVelocity << std::endl;
#endif

    // Publish event for SoundManager/Audio listener
    EventBus::getInstance().notify(EventType::PLAYER_JUMPED);
  }
}

void Mario::rebuildFixture() {
  sf::Vector2f targetSize = (m_marioState == MarioState::SMALL)
                                ? SMALL_MARIO_SIZE
                                : SUPER_MARIO_SIZE;
  m_size = targetSize;

  if (!m_body)
    return;

  // Remove existing fixtures
  for (b2Fixture *f = m_body->GetFixtureList(); f;) {
    b2Fixture *next = f->GetNext();
    m_body->DestroyFixture(f);
    f = next;
  }

  // Create new shape with 0.0f friction to prevent wall sticking
  b2PolygonShape dynamicBox;
  dynamicBox.SetAsBox(PhysicsEngine::pixelsToMeters(targetSize.x / 2.0f),
                      PhysicsEngine::pixelsToMeters(targetSize.y / 2.0f));

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &dynamicBox;
  fixtureDef.density = MARIO_FIXTURE_DENSITY;
  fixtureDef.friction = MARIO_FIXTURE_FRICTION; // Prevent wall sticking
  m_body->CreateFixture(&fixtureDef);

#ifdef DEBUG
  std::cout << "[DEBUG][Mario] Rebuilt fixture for state size: (" 
            << targetSize.x << ", " << targetSize.y << ")" << std::endl;
#endif
}

void Mario::powerUp(MarioState state) {
  m_marioState = state;
  EventBus::getInstance().notify(EventType::PLAYER_POWER_UP);
  rebuildFixture();
}

void Mario::powerDown() {
  if (m_marioState == MarioState::FIRE) {
    m_marioState = MarioState::SUPER;
    EventBus::getInstance().notify(EventType::PLAYER_POWER_DOWN);
    rebuildFixture();
  } else if (m_marioState == MarioState::SUPER) {
    m_marioState = MarioState::SMALL;
    EventBus::getInstance().notify(EventType::PLAYER_POWER_DOWN);
    rebuildFixture();
  } else {
    takeDamage(FATAL_DAMAGE); // Small Mario dies
    EventBus::getInstance().notify(EventType::PLAYER_DIED);
  }
}

MarioState Mario::getMarioState() const { return m_marioState; }

void Mario::setMarioState(MarioState state) {
  m_marioState = state;
  rebuildFixture();
}

bool Mario::canShootFireBall() const {
  return m_marioState == MarioState::FIRE;
}
