/**
 * @file Mario.cpp
 * @author TV3 (Bảo)
 * @brief Mario character implementation with authentic NES movement physics and states
 * @note Sprint 4 merge: lives, skidding, FireBall (TV3) + score, invincibility (develop)
 */

#include "entities/Mario.h"
#include <algorithm>
#include <iostream>
#include <cmath>
#include "physics/PhysicsEngine.h"
#include "level/TileMap.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "core/AnimationSystem.h"
#include "core/SpriteFrames.h"

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
constexpr float DAMAGE_INVINCIBILITY_DURATION = 1.0f;
constexpr float GROUND_NORMAL_Y_THRESHOLD = 0.8f;
constexpr float MAX_GROUND_NORMAL_X = 0.5f;

// Dimensions & Physics Constants
const sf::Vector2f DEFAULT_MARIO_POSITION(100.f, 100.f);
const sf::Vector2f SMALL_MARIO_SIZE(32.f, 32.f);
const sf::Vector2f SUPER_MARIO_SIZE(32.f, 64.f);
constexpr float MARIO_FIXTURE_DENSITY = 1.0f;
constexpr float MARIO_FIXTURE_FRICTION = 0.0f;

constexpr const char* MARIO_TEXTURE_PATH = "assets/textures/mario/MarioLuigi.png";

// Helper: register animation clips for the current MarioState
void setupAnimationsForState(AnimationSystem& animSys, MarioState state) {
    switch (state) {
    case MarioState::SMALL: {
        namespace F = SpriteFrames::SmallMario;
        animSys.addAnimation("idle",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::IDLE},  1.f));
        animSys.addAnimation("walk",  AnimationSystem::createManualAnimation(F::walkFrames(), 0.1f));
        animSys.addAnimation("jump",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::JUMP},  1.f));
        animSys.addAnimation("skid",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::SKID},  1.f));
        animSys.addAnimation("death", AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::DEATH}, 1.f, false));
        break;
    }
    case MarioState::SUPER: {
        namespace F = SpriteFrames::BigMario;
        animSys.addAnimation("idle",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::IDLE},  1.f));
        animSys.addAnimation("walk",  AnimationSystem::createManualAnimation(F::walkFrames(), 0.1f));
        animSys.addAnimation("jump",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::JUMP},  1.f));
        animSys.addAnimation("skid",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::SKID},  1.f));
        animSys.addAnimation("death", AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{SpriteFrames::SmallMario::DEATH}, 1.f, false));
        break;
    }
    case MarioState::FIRE: {
        namespace F = SpriteFrames::FireBigMario;
        animSys.addAnimation("idle",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::IDLE},  1.f));
        animSys.addAnimation("walk",  AnimationSystem::createManualAnimation(F::walkFrames(), 0.1f));
        animSys.addAnimation("jump",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::JUMP},  1.f));
        animSys.addAnimation("skid",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::SKID},  1.f));
        animSys.addAnimation("death", AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{SpriteFrames::SmallMario::DEATH}, 1.f, false));
        break;
    }
    }
}
} // namespace

Mario::Mario()
    : Character(DEFAULT_MARIO_POSITION, SMALL_MARIO_SIZE, DEFAULT_MARIO_HEALTH),
      m_marioState(MarioState::SMALL),
      m_jumpForce(DEFAULT_JUMP_FORCE),
      m_moveSpeed(WALK_MAX_SPEED),
      m_score(0),
      m_coinCount(0),
      m_isInvincible(false),
      m_invincibilityTimer(0.f),
      m_lives(DEFAULT_MARIO_LIVES),
      m_isRunning(false),
      m_isSkidding(false),
      m_wasJumpPressed(false),
      m_pendingPowerDown(false),
      m_respawnPosition(DEFAULT_MARIO_POSITION) {
    setupAnimationsForState(*m_animationSystem, m_marioState);
    playAnimation("idle");
    setSprite(MARIO_TEXTURE_PATH);
}

Mario::Mario(const sf::Vector2f &position, const sf::Vector2f &size)
    : Character(position, size, DEFAULT_MARIO_HEALTH),
      m_marioState(MarioState::SMALL),
      m_jumpForce(DEFAULT_JUMP_FORCE),
      m_moveSpeed(WALK_MAX_SPEED),
      m_score(0),
      m_coinCount(0),
      m_isInvincible(false),
      m_invincibilityTimer(0.f),
      m_lives(DEFAULT_MARIO_LIVES),
      m_isRunning(false),
      m_isSkidding(false),
      m_wasJumpPressed(false),
      m_pendingPowerDown(false),
      m_respawnPosition(position) {
    setupAnimationsForState(*m_animationSystem, m_marioState);
    playAnimation("idle");
    setSprite(MARIO_TEXTURE_PATH);
}

void Mario::update(float dt) {
  if (!m_active) return;

  if (m_pendingFixtureRebuild) {
    rebuildFixture();
  }

  // CRITICAL: Sync Box2D physics before doing custom movement/clamp logic
  syncPhysics();

  if (m_pendingPowerDown) {
    m_pendingPowerDown = false;
    powerDown();
  }

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

  // Animation state machine (develop)
  if (!isGrounded()) {
      playAnimation("jump");
  } else if (m_isSkidding) {
      playAnimation("skid");
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

  // Handle scaling and flipping manually in update for now
  if (m_sprite) {
      float scaleX = (m_facingDirection == Direction::LEFT) ? -2.f : 2.f;
      m_sprite->setScale({scaleX, 2.f});
      if (m_facingDirection == Direction::LEFT) {
          m_sprite->setOrigin({16.f, 0.f});
      } else {
          m_sprite->setOrigin({0.f, 0.f});
      }
  }
}

void Mario::preparePhysics(float dt) {
  if (!m_active || !m_body) {
    return;
  }

  // Apply player input before Box2D advances so the current frame reacts immediately.
  applyMovementPhysics(dt, m_inputDirX, m_isRunning, m_jumpRequested, false);
  m_jumpRequested = false;
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
    if (std::abs(newVx) < 1.0f) {
      newVx = 0.0f;
    }
  }

  if (jumpKeyPressed) {
    float jumpVelocityMeters = -PhysicsEngine::pixelsToMeters(m_jumpForce);
    currentVy = jumpVelocityMeters;
    clearGroundedState();

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
    newVx = std::clamp(newVx, -targetMaxSpeed, targetMaxSpeed);
  } else {
    float airFrictionStep = AIR_FRICTION * dt;
    if (newVx > 0.0f) {
      newVx = std::max(0.0f, newVx - airFrictionStep);
    } else if (newVx < 0.0f) {
      newVx = std::min(0.0f, newVx + airFrictionStep);
    }
    if (std::abs(newVx) < 1.0f) {
      newVx = 0.0f;
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

  if (m_body->GetWorld() && m_body->GetWorld()->IsLocked()) {
    m_pendingFixtureRebuild = true;
    return;
  }
  m_pendingFixtureRebuild = false;

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

  m_jumpRequested = true;
}

void Mario::moveLeft() {
  setMoveIntent(-1.0f);
}

void Mario::moveRight() {
  setMoveIntent(1.0f);
}

void Mario::stopMoving() {
  setMoveIntent(0.0f);
}

void Mario::setMoveIntent(float inputDirection) {
  m_inputDirX = std::clamp(inputDirection, -1.0f, 1.0f);
  if (m_inputDirX < 0.0f) {
    setFacingDirection(Direction::LEFT);
  } else if (m_inputDirX > 0.0f) {
    setFacingDirection(Direction::RIGHT);
  }
}


void Mario::powerUp(MarioState state) {
  m_marioState = state;
  setupAnimationsForState(*m_animationSystem, m_marioState);
  playAnimation("idle");
  EventBus::getInstance().notify(EventType::PLAYER_POWER_UP);
  rebuildFixture();
}

void Mario::powerDown() {
  if (m_isInvincible) {
    return;
  }
  if (m_body && m_body->GetWorld() && m_body->GetWorld()->IsLocked()) {
    queuePowerDown();
    return;
  }

  if (m_marioState == MarioState::FIRE) {
    m_marioState = MarioState::SUPER;
    EventBus::getInstance().notify(EventType::PLAYER_POWER_DOWN);
    rebuildFixture();
    setInvincible(DAMAGE_INVINCIBILITY_DURATION);
  } else if (m_marioState == MarioState::SUPER) {
    m_marioState = MarioState::SMALL;
    EventBus::getInstance().notify(EventType::PLAYER_POWER_DOWN);
    rebuildFixture();
    setInvincible(DAMAGE_INVINCIBILITY_DURATION);
  } else {
    loseLife();
  }
}

void Mario::queuePowerDown() {
  if (!m_isInvincible) {
    m_pendingPowerDown = true;
  }
}

void Mario::loseLife() {
  if (m_lives > 0) {
    m_lives--;
  }

#ifdef DEBUG
  std::cout << "[DEBUG][Mario] Mario died. Lives remaining: " << m_lives << std::endl;
#endif

  if (m_lives > 0) {
    respawn(m_respawnPosition);
    setInvincible(DAMAGE_INVINCIBILITY_DURATION);
  } else {
    takeDamage(FATAL_DAMAGE);
    m_active = false;
    EventBus::getInstance().notify(EventType::PLAYER_DIED);
  }
}

void Mario::respawn(const sf::Vector2f& spawnPosition) {
  m_marioState = MarioState::SMALL;
  m_size = SMALL_MARIO_SIZE;
  m_health = DEFAULT_MARIO_HEALTH;
  m_active = true;
  clearGroundedState();
  setPosition(spawnPosition);
  if (m_body) {
    m_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
    m_body->SetAngularVelocity(0.f);
    m_body->SetAwake(true);
  }
  setupAnimationsForState(*m_animationSystem, m_marioState);
  playAnimation("idle");
  rebuildFixture();
}

void Mario::setRespawnPosition(const sf::Vector2f& spawnPosition) {
  m_respawnPosition = spawnPosition;
}

MarioState Mario::getMarioState() const { return m_marioState; }

void Mario::setMarioState(MarioState state) {
  m_marioState = state;
  rebuildFixture();
}

void Mario::addScore(int points) {
  m_score += points;
}

void Mario::collectCoin(int scoreValue) {
  ++m_coinCount;
  addScore(scoreValue);
  EventBus::getInstance().notify(EventType::COIN_COLLECTED);
}

int Mario::getScore() const {
  return m_score;
}

int Mario::getCoinCount() const {
  return m_coinCount;
}

void Mario::setInvincible(float duration) {
  m_isInvincible = true;
  m_invincibilityTimer = duration;
}

void Mario::updateInvincibility(float dt) {
  if (!m_isInvincible) {
    if (m_sprite) {
      m_sprite->setColor(sf::Color::White); // Ensure visible
    }
    return;
  }
  
  m_invincibilityTimer -= dt;
  
  // Flashing effect: toggle opacity every 0.1 seconds
  if (m_sprite) {
    int ms = static_cast<int>(m_invincibilityTimer * 1000);
    if ((ms / 100) % 2 == 0) {
      m_sprite->setColor(sf::Color::Transparent);
    } else {
      m_sprite->setColor(sf::Color::White);
    }
  }

  if (m_invincibilityTimer <= 0.f) {
    m_isInvincible = false;
    m_invincibilityTimer = 0.f;
    if (m_sprite) {
      m_sprite->setColor(sf::Color::White);
    }
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

void Mario::refreshGroundedState() {
  if (!m_body) {
    setGrounded(false);
    return;
  }

  bool grounded = false;
  for (b2ContactEdge* edge = m_body->GetContactList(); edge; edge = edge->next) {
    b2Contact* contact = edge->contact;
    if (!contact || !contact->IsTouching()) {
      continue;
    }

    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();
    b2Body* otherBody = (bodyA == m_body) ? bodyB : bodyA;
    if (!otherBody) {
      continue;
    }

    const uintptr_t userData = otherBody->GetUserData().pointer;
    Entity* otherEntity = (userData != 0 && !TileMap::isTileUserData(userData))
                              ? reinterpret_cast<Entity*>(userData)
                              : nullptr;
    if (otherEntity && (otherEntity->isEnemy() || otherEntity->isItem())) {
      continue;
    }

    b2WorldManifold manifold;
    contact->GetWorldManifold(&manifold);
    b2Vec2 normal = manifold.normal;
    if (bodyB == m_body) {
      normal = -normal;
    }

    if (normal.y > GROUND_NORMAL_Y_THRESHOLD &&
        std::abs(normal.x) < MAX_GROUND_NORMAL_X) {
      grounded = true;
      break;
    }
  }

  setGrounded(grounded);
}

void Mario::clearGroundedState() {
  setGrounded(false);
}
