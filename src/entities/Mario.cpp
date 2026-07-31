/**
 * @file Mario.cpp
 * @author TV3
 * @brief Mario character implementation with authentic NES movement physics and states
 */

#include "entities/Mario.h"
#include <iostream>
#include <cmath>
#include "physics/PhysicsEngine.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"

namespace {
constexpr int DEFAULT_MARIO_HEALTH = 100;
constexpr int DEFAULT_MARIO_LIVES = 3;
constexpr float DEFAULT_JUMP_FORCE = 460.f;
constexpr float MAX_FALL_SPEED = 600.f;
constexpr float PIT_DEATH_Y_THRESHOLD = 800.f;
constexpr int FATAL_DAMAGE = 100;

// Authentic Mario Movement Physics Constants (in pixels/sec)
constexpr float WALK_MAX_SPEED = 180.f;            // ~6.0 m/s
constexpr float RUN_MAX_SPEED = 340.f;             // ~11.3 m/s
constexpr float GROUND_ACCEL = 900.f;             // Walk acceleration
constexpr float RUN_ACCEL = 1400.f;               // Sprint acceleration
constexpr float GROUND_FRICTION = 1100.f;         // Deceleration when idle on ground
constexpr float SKID_FRICTION = 2200.f;           // Deceleration when reversing direction
constexpr float AIR_ACCEL = 450.f;                // Reduced horizontal control in air
constexpr float AIR_FRICTION = 150.f;              // Low air drag preserving jump momentum
constexpr float SHORT_HOP_CUTOFF = 0.5f;          // Velocity multiplier on early jump key release
constexpr float SKID_SPEED_THRESHOLD = 15.0f;     // Minimum speed required to trigger skidding
constexpr float ASCENDING_VEL_THRESHOLD = -0.5f;   // Threshold to detect upward jump ascent

// Dimensions & Physics Constants
const sf::Vector2f DEFAULT_MARIO_POSITION(100.f, 100.f);
const sf::Vector2f SMALL_MARIO_SIZE(32.f, 32.f);
const sf::Vector2f SUPER_MARIO_SIZE(32.f, 64.f);
constexpr float MARIO_FIXTURE_DENSITY = 1.0f;
constexpr float MARIO_FIXTURE_FRICTION = 0.0f;     // Zero friction to prevent wall sticking
} // namespace

Mario::Mario()
    : Character(DEFAULT_MARIO_POSITION, SMALL_MARIO_SIZE, DEFAULT_MARIO_HEALTH),
      m_marioState(MarioState::SMALL),
      m_jumpForce(DEFAULT_JUMP_FORCE),
      m_moveSpeed(WALK_MAX_SPEED),
      m_lives(DEFAULT_MARIO_LIVES),
      m_isRunning(false),
      m_isSkidding(false),
      m_wasJumpPressed(false) {
    setSprite("assets/textures/mario/idle.png");
}

Mario::Mario(const sf::Vector2f &position, const sf::Vector2f &size)
    : Character(position, size, DEFAULT_MARIO_HEALTH),
      m_marioState(MarioState::SMALL),
      m_jumpForce(DEFAULT_JUMP_FORCE),
      m_moveSpeed(WALK_MAX_SPEED),
      m_lives(DEFAULT_MARIO_LIVES),
      m_isRunning(false),
      m_isSkidding(false),
      m_wasJumpPressed(false) {
    setSprite("assets/textures/mario/idle.png");
}

void Mario::update(float dt) {
  (void)dt;
  if (!m_active) return;

  // Clamp terminal fall velocity to prevent AABB tunneling through floor tiles
  if (m_body) {
    b2Vec2 velocity = m_body->GetLinearVelocity();
    float maxFallMeters = PhysicsEngine::pixelsToMeters(MAX_FALL_SPEED);
    if (velocity.y > maxFallMeters) {
      m_body->SetLinearVelocity(b2Vec2(velocity.x, maxFallMeters));
    }
  }

  // Sync position with Box2D body
  syncPhysics();

  // Pit fall check (death threshold)
  if (m_position.y > PIT_DEATH_Y_THRESHOLD) {
    loseLife();
  }
}

void Mario::handleInput() {
  if (!m_body || !m_active)
    return;

  // Input polling decoupled — actions are handled via InputHandler commands (Task 3.2).
  constexpr float FIXED_DT = 1.0f / 60.0f;
  applyMovementPhysics(FIXED_DT, 0.0f, m_isRunning, false, false);
}

void Mario::applyGroundPhysics(float dt, float inputDirX, bool isRunningInput, bool jumpKeyPressed, float& currentVy, float& newVx, float targetMaxSpeed) {
  if (inputDirX != 0.0f) {
    if ((newVx > SKID_SPEED_THRESHOLD && inputDirX < 0.0f) || (newVx < -SKID_SPEED_THRESHOLD && inputDirX > 0.0f)) {
      m_isSkidding = true;
      float skidStep = SKID_FRICTION * dt;
      if (newVx > 0.0f) {
        newVx = std::max(0.0f, newVx - skidStep);
      } else {
        newVx = std::min(0.0f, newVx + skidStep);
      }
    } else {
      m_isSkidding = false;
      float accelRate = isRunningInput ? RUN_ACCEL : GROUND_ACCEL;
      newVx += inputDirX * accelRate * dt;

      if (newVx > targetMaxSpeed) newVx = targetMaxSpeed;
      if (newVx < -targetMaxSpeed) newVx = -targetMaxSpeed;
    }
  } else {
    m_isSkidding = false;
    float frictionStep = GROUND_FRICTION * dt;
    if (newVx > 0.0f) {
      newVx = std::max(0.0f, newVx - frictionStep);
    } else if (newVx < 0.0f) {
      newVx = std::min(0.0f, newVx + frictionStep);
    }
  }

  if (jumpKeyPressed) {
    float jumpVelocityMeters = -PhysicsEngine::pixelsToMeters(m_jumpForce);
    currentVy = jumpVelocityMeters;
    setGrounded(false);

#ifdef DEBUG
    std::cout << "[DEBUG][Mario] Jump executed with velocity: " << jumpVelocityMeters << std::endl;
#endif

    EventBus::getInstance().notify(EventType::PLAYER_JUMPED);
  }
}

void Mario::applyAirPhysics(float dt, float inputDirX, bool jumpKeyReleased, float& currentVy, float& newVx, float targetMaxSpeed) {
  m_isSkidding = false;

  if (inputDirX != 0.0f) {
    newVx += inputDirX * AIR_ACCEL * dt;

    float maxAirSpeed = std::max(targetMaxSpeed, std::abs(newVx));
    if (newVx > maxAirSpeed) newVx = maxAirSpeed;
    if (newVx < -maxAirSpeed) newVx = -maxAirSpeed;
  } else {
    float airFrictionStep = AIR_FRICTION * dt;
    if (newVx > 0.0f) {
      newVx = std::max(0.0f, newVx - airFrictionStep);
    } else if (newVx < 0.0f) {
      newVx = std::min(0.0f, newVx + airFrictionStep);
    }
  }

  if (jumpKeyReleased && currentVy < ASCENDING_VEL_THRESHOLD) {
    currentVy *= SHORT_HOP_CUTOFF;
  }
}

void Mario::applyMovementPhysics(float dt, float inputDirX, bool isRunningInput, bool jumpKeyPressed, bool jumpKeyReleased) {
  if (!m_body) return;

  b2Vec2 currentVelMeters = m_body->GetLinearVelocity();
  float currentVx = PhysicsEngine::metersToPixels(currentVelMeters.x);
  float currentVy = currentVelMeters.y;

  m_isRunning = isRunningInput;
  float targetMaxSpeed = isRunningInput ? RUN_MAX_SPEED : WALK_MAX_SPEED;
  float newVx = currentVx;

  if (isGrounded()) {
    applyGroundPhysics(dt, inputDirX, isRunningInput, jumpKeyPressed, currentVy, newVx, targetMaxSpeed);
  } else {
    applyAirPhysics(dt, inputDirX, jumpKeyReleased, currentVy, newVx, targetMaxSpeed);
  }

  float newVxMeters = PhysicsEngine::pixelsToMeters(newVx);
  m_body->SetLinearVelocity(b2Vec2(newVxMeters, currentVy));
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
  fixtureDef.friction = MARIO_FIXTURE_FRICTION;
  m_body->CreateFixture(&fixtureDef);

#ifdef DEBUG
  std::cout << "[DEBUG][Mario] Rebuilt fixture for state size: (" 
            << targetSize.x << ", " << targetSize.y << ")" << std::endl;
#endif
}

void Mario::jump() {
  if (!m_body || !isGrounded())
    return;

  float jumpVelocity = -PhysicsEngine::pixelsToMeters(m_jumpForce);
  m_body->SetLinearVelocity(
      b2Vec2(m_body->GetLinearVelocity().x, jumpVelocity));
  setGrounded(false);
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
    loseLife();
  }
}

void Mario::loseLife() {
  if (m_lives > 0) {
    m_lives--;
  }

  EventBus::getInstance().notify(EventType::PLAYER_DIED);

#ifdef DEBUG
  std::cout << "[DEBUG][Mario] Mario died. Lives remaining: " << m_lives << std::endl;
#endif

  if (m_lives > 0) {
    respawn(DEFAULT_MARIO_POSITION);
  } else {
    takeDamage(FATAL_DAMAGE);
    m_active = false;
  }
}

void Mario::respawn(const sf::Vector2f& spawnPosition) {
  m_marioState = MarioState::SMALL;
  m_health = DEFAULT_MARIO_HEALTH;
  m_active = true;
  setPosition(spawnPosition);
  rebuildFixture();

  if (m_body) {
    m_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
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

int Mario::getLives() const {
  return m_lives;
}

void Mario::setLives(int lives) {
  m_lives = lives;
}

bool Mario::isRunning() const {
  return m_isRunning;
}

bool Mario::isSkidding() const {
  return m_isSkidding;
}
