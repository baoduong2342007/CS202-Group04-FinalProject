/**
 * @file Mario.cpp
 * @author TV3 (Bảo)
 * @brief Mario character implementation with authentic NES movement physics and states
 * @note Sprint 4 merge: lives, skidding, FireBall (TV3) + score, invincibility (develop)
 */

#include "entities/Mario.h"
#include <iostream>
#include <cmath>
#include "physics/PhysicsEngine.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "core/AnimationSystem.h"

namespace {
constexpr int DEFAULT_MARIO_HEALTH = 100;
constexpr int DEFAULT_MARIO_LIVES = 3;
constexpr float DEFAULT_JUMP_FORCE = 460.f;
constexpr float MAX_FALL_SPEED = 600.f;
constexpr float PIT_DEATH_Y_THRESHOLD = 800.f;
constexpr int FATAL_DAMAGE = 100;

// Authentic Mario Movement Physics Constants (in pixels/sec)
constexpr float WALK_MAX_SPEED = 180.f;
constexpr float RUN_MAX_SPEED = 340.f;
constexpr float GROUND_ACCEL = 900.f;
constexpr float RUN_ACCEL = 1400.f;
constexpr float GROUND_FRICTION = 1100.f;
constexpr float SKID_FRICTION = 2200.f;
constexpr float AIR_ACCEL = 450.f;
constexpr float AIR_FRICTION = 150.f;
constexpr float SHORT_HOP_CUTOFF = 0.5f;
constexpr float SKID_SPEED_THRESHOLD = 15.0f;
constexpr float ASCENDING_VEL_THRESHOLD = -0.5f;

// Dimensions & Physics Constants
const sf::Vector2f DEFAULT_MARIO_POSITION(100.f, 100.f);
const sf::Vector2f SMALL_MARIO_SIZE(32.f, 32.f);
const sf::Vector2f SUPER_MARIO_SIZE(32.f, 64.f);
constexpr float MARIO_FIXTURE_DENSITY = 1.0f;
constexpr float MARIO_FIXTURE_FRICTION = 0.0f;
} // namespace

Mario::Mario()
    : Character(DEFAULT_MARIO_POSITION, SMALL_MARIO_SIZE, DEFAULT_MARIO_HEALTH),
      m_marioState(MarioState::SMALL),
      m_jumpForce(DEFAULT_JUMP_FORCE),
      m_moveSpeed(WALK_MAX_SPEED),
      m_score(0),
      m_isInvincible(false),
      m_invincibilityTimer(0.f),
      m_lives(DEFAULT_MARIO_LIVES),
      m_isRunning(false),
      m_isSkidding(false),
      m_wasJumpPressed(false) {
    m_animationSystem->addAnimation("idle", AnimationSystem::createGridAnimation(0, 0, 32, 32, 1, 1.f));
    m_animationSystem->addAnimation("walk", AnimationSystem::createGridAnimation(0, 0, 32, 32, 3, 0.1f));
    m_animationSystem->addAnimation("jump", AnimationSystem::createGridAnimation(96, 0, 32, 32, 1, 1.f));
    playAnimation("idle");
    setSprite("assets/textures/mario/idle.png");
}

Mario::Mario(const sf::Vector2f &position, const sf::Vector2f &size)
    : Character(position, size, DEFAULT_MARIO_HEALTH),
      m_marioState(MarioState::SMALL),
      m_jumpForce(DEFAULT_JUMP_FORCE),
      m_moveSpeed(WALK_MAX_SPEED),
      m_score(0),
      m_isInvincible(false),
      m_invincibilityTimer(0.f),
      m_lives(DEFAULT_MARIO_LIVES),
      m_isRunning(false),
      m_isSkidding(false),
      m_wasJumpPressed(false) {
    m_animationSystem->addAnimation("idle", AnimationSystem::createGridAnimation(0, 0, 32, 32, 1, 1.f));
    m_animationSystem->addAnimation("walk", AnimationSystem::createGridAnimation(0, 0, 32, 32, 3, 0.1f));
    m_animationSystem->addAnimation("jump", AnimationSystem::createGridAnimation(96, 0, 32, 32, 1, 1.f));
    playAnimation("idle");
    setSprite("assets/textures/mario/idle.png");
}

void Mario::update(float dt) {
  if (!m_active) return;

  // Tick invincibility timer (develop)
  updateInvincibility(dt);

  // Clamp terminal fall velocity to prevent AABB tunneling (TV3)
  if (m_body) {
    b2Vec2 velocity = m_body->GetLinearVelocity();
    float maxFallMeters = PhysicsEngine::pixelsToMeters(MAX_FALL_SPEED);
    if (velocity.y > maxFallMeters) {
      m_body->SetLinearVelocity(b2Vec2(velocity.x, maxFallMeters));
    }
  }

  // Sync position with Box2D body
  syncPhysics();

  // Animation state machine (develop)
  if (!isGrounded()) {
      playAnimation("jump");
  } else if (std::abs(getVelocity().x) > 5.f) {
      playAnimation("walk");
  } else {
      playAnimation("idle");
  }
  updateAnimation(dt);

  // Pit fall check (TV3)
  if (m_position.y > PIT_DEATH_Y_THRESHOLD) {
    loseLife();
  }
}

// DEPRECATED: Replaced by InputHandler (Command Pattern) in Game::update().
// Kept as fallback for debugging. Remove after team confirms InputHandler works.
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
  EventBus::getInstance().notify(EventType::PLAYER_POWER_UP);
  rebuildFixture();
}

void Mario::powerDown() {
  if (m_isInvincible) {
    return;
  }
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

void Mario::addScore(int points) {
  m_score += points;
}

int Mario::getScore() const {
  return m_score;
}

void Mario::setInvincible(float duration) {
  m_isInvincible = true;
  m_invincibilityTimer = duration;
}

void Mario::updateInvincibility(float dt) {
  if (!m_isInvincible) {
    return;
  }
  m_invincibilityTimer -= dt;
  if (m_invincibilityTimer <= 0.f) {
    m_isInvincible = false;
    m_invincibilityTimer = 0.f;
  }
}

bool Mario::isInvincible() const {
  return m_isInvincible;
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