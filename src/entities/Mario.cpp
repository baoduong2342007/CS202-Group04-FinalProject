/**
 * @file Mario.cpp
 * @author TV3 (Bảo)
 * @brief Mario character implementation with authentic NES movement physics and states
 * @note Sprint 4 merge: lives, skidding, FireBall (TV3) + score, invincibility (develop)
 */

#include "entities/Mario.h"
#include <iostream>
#include <cmath>
#include "entities/FireBall.h"
#include "entities/Enemy.h"
#include "items/Item.h"
#include "physics/PhysicsEngine.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "core/AnimationSystem.h"
#include "states/SmallMarioState.h"
#include "states/SuperMarioState.h"
#include "states/FireMarioState.h"

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
constexpr float TOP_STOMP_NORMAL_THRESHOLD = 0.8f;
constexpr float MAX_WALL_NORMAL_X = 0.5f;
constexpr float STOMP_BOUNCE_SPEED = 300.f;

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
      m_statePattern(std::make_unique<SmallMarioState>()),
      m_jumpForce(DEFAULT_JUMP_FORCE),
      m_moveSpeed(WALK_MAX_SPEED),
      m_score(0),
      m_coinCount(0),
      m_isInvincible(false),
      m_invincibilityTimer(0.f),
      m_lives(DEFAULT_MARIO_LIVES),
      m_isDying(false),
      m_deathTimer(0.f),
      m_isRunning(false),
      m_isSkidding(false),
      m_wasJumpPressed(false),
      m_isTransforming(false),
      m_transformTimer(0.f) {
    m_animationSystem->addAnimation("idle", AnimationSystem::createGridAnimation(0, 0, 32, 32, 1, 1.f));
    m_animationSystem->addAnimation("walk", AnimationSystem::createGridAnimation(0, 0, 32, 32, 3, 0.1f));
    m_animationSystem->addAnimation("jump", AnimationSystem::createGridAnimation(96, 0, 32, 32, 1, 1.f));
    m_animationSystem->addAnimation("death", AnimationSystem::createGridAnimation(128, 0, 32, 32, 1, 1.f));
    m_animationSystem->addAnimation("spawn", AnimationSystem::createGridAnimation(160, 0, 32, 32, 3, 0.15f));
    playAnimation("idle");
    setSprite("assets/textures/mario/MarioLuigi.png");
}

Mario::Mario(const sf::Vector2f &position, const sf::Vector2f &size)
    : Character(position, size, DEFAULT_MARIO_HEALTH),
      m_marioState(MarioState::SMALL),
      m_statePattern(std::make_unique<SmallMarioState>()),
      m_jumpForce(DEFAULT_JUMP_FORCE),
      m_moveSpeed(WALK_MAX_SPEED),
      m_score(0),
      m_coinCount(0),
      m_isInvincible(false),
      m_invincibilityTimer(0.f),
      m_lives(DEFAULT_MARIO_LIVES),
      m_isDying(false),
      m_deathTimer(0.f),
      m_isRunning(false),
      m_isSkidding(false),
      m_wasJumpPressed(false),
      m_isTransforming(false),
      m_transformTimer(0.f) {
    m_animationSystem->addAnimation("idle", AnimationSystem::createGridAnimation(0, 0, 32, 32, 1, 1.f));
    m_animationSystem->addAnimation("walk", AnimationSystem::createGridAnimation(0, 0, 32, 32, 3, 0.1f));
    m_animationSystem->addAnimation("jump", AnimationSystem::createGridAnimation(96, 0, 32, 32, 1, 1.f));
    m_animationSystem->addAnimation("death", AnimationSystem::createGridAnimation(128, 0, 32, 32, 1, 1.f));
    m_animationSystem->addAnimation("spawn", AnimationSystem::createGridAnimation(160, 0, 32, 32, 3, 0.15f));
    playAnimation("idle");
    setSprite("assets/textures/mario/MarioLuigi.png");
}

void Mario::update(float dt) {
  if (!m_active) return;

  // Handle transformation transition animation (0.5s freeze & flash)
  if (m_isTransforming) {
    m_transformTimer -= dt;
    if (m_body) {
      b2Vec2 currentVel = m_body->GetLinearVelocity();
      m_body->SetLinearVelocity(b2Vec2(0.f, currentVel.y));
    }
    if (m_sprite) {
      int ms = static_cast<int>(m_transformTimer * 1000);
      m_sprite->setColor((ms / 50) % 2 == 0 ? sf::Color::Transparent : sf::Color::White);
    }
    if (m_transformTimer <= 0.f) {
      m_isTransforming = false;
      m_transformTimer = 0.f;
      if (m_sprite) {
        m_sprite->setColor(m_marioState == MarioState::FIRE ? sf::Color(255, 140, 0) : sf::Color::White);
      }
    }
    return;
  }

  // Handle death animation phase
  if (m_isDying) {
    if (m_body) {
      m_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
    }
    m_deathTimer -= dt;
    updateAnimation(dt);
    if (m_deathTimer <= 0.f) {
      m_isDying = false;
      if (m_lives > 0) {
        respawn(DEFAULT_MARIO_POSITION);
      } else {
        takeDamage(FATAL_DAMAGE);
        m_active = false;
      }
    }
    return;
  }

  // CRITICAL: Sync Box2D physics before doing custom movement/clamp logic
  syncPhysics();

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
  if (!m_body || !m_active || m_isDying)
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
  if (!m_body || m_isDying) return;

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
  if (!m_body || !isGrounded() || m_isDying)
    return;

  float jumpVelocity = -PhysicsEngine::pixelsToMeters(m_jumpForce);
  m_body->SetLinearVelocity(
      b2Vec2(m_body->GetLinearVelocity().x, jumpVelocity));
  setGrounded(false);

  EventBus::getInstance().notify(EventType::PLAYER_JUMPED);
}

void Mario::moveLeft() {
  if (!m_body || m_isDying)
    return;

  float desiredXVelocity = -PhysicsEngine::pixelsToMeters(m_moveSpeed);
  m_body->SetLinearVelocity(
      b2Vec2(desiredXVelocity, m_body->GetLinearVelocity().y));
  setFacingDirection(Direction::LEFT);
}

void Mario::moveRight() {
  if (!m_body || m_isDying)
    return;

  float desiredXVelocity = PhysicsEngine::pixelsToMeters(m_moveSpeed);
  m_body->SetLinearVelocity(
      b2Vec2(desiredXVelocity, m_body->GetLinearVelocity().y));
  setFacingDirection(Direction::RIGHT);
}

void Mario::stopMoving() {
  if (!m_body || m_isDying)
    return;

  m_body->SetLinearVelocity(
      b2Vec2(0.0f, m_body->GetLinearVelocity().y));
}

void Mario::powerUp(MarioState state) {
  if (m_marioState == state) return;

  bool wasSmall = (m_marioState == MarioState::SMALL);
  m_marioState = state;

  if (state == MarioState::SMALL) {
    m_statePattern = std::make_unique<SmallMarioState>();
  } else if (state == MarioState::SUPER) {
    m_statePattern = std::make_unique<SuperMarioState>();
  } else if (state == MarioState::FIRE) {
    m_statePattern = std::make_unique<FireMarioState>();
  }

  // If growing from Small to Super/Fire, offset Box2D body upward by 16px (0.5m)
  if (wasSmall && m_body) {
    b2Vec2 currentPos = m_body->GetPosition();
    m_body->SetTransform(b2Vec2(currentPos.x, currentPos.y - PhysicsEngine::pixelsToMeters(16.f)), m_body->GetAngle());
  }

  m_isTransforming = true;
  m_transformTimer = 0.5f;

  EventBus::getInstance().notify(EventType::PLAYER_POWER_UP);
  rebuildFixture();

  if (m_sprite) {
    if (m_marioState == MarioState::FIRE) {
      m_sprite->setColor(sf::Color(255, 140, 0)); // Fire Orange visual tint
    } else {
      m_sprite->setColor(sf::Color::White);
    }
  }
}

void Mario::powerDown() {
  if (m_isInvincible || m_isDying || m_isTransforming) {
    return;
  }
  if (m_marioState == MarioState::FIRE) {
    powerUp(MarioState::SUPER);
    setInvincible(2.0f);
    EventBus::getInstance().notify(EventType::PLAYER_POWER_DOWN);
  } else if (m_marioState == MarioState::SUPER) {
    powerUp(MarioState::SMALL);
    setInvincible(2.0f);
    EventBus::getInstance().notify(EventType::PLAYER_POWER_DOWN);
  } else {
    loseLife();
  }
}

void Mario::loseLife() {
  if (m_isDying) return;

  m_isDying = true;
  m_deathTimer = 0.5f;
  playAnimation("death");

  if (m_body) {
    m_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
  }

  if (m_lives > 0) {
    m_lives--;
  }
  EventBus::getInstance().notify(EventType::PLAYER_DIED);

#ifdef DEBUG
  std::cout << "[DEBUG][Mario] Mario died. Lives remaining: " << m_lives << std::endl;
#endif
}

void Mario::respawn(const sf::Vector2f& spawnPosition) {
  m_marioState = MarioState::SMALL;
  m_health = DEFAULT_MARIO_HEALTH;
  m_active = true;
  m_isDying = false;
  playAnimation("spawn");
  setPosition(spawnPosition);
  rebuildFixture();

  if (m_sprite) {
    m_sprite->setColor(sf::Color::White);
  }

  if (m_body) {
    m_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
  }
}

MarioState Mario::getMarioState() const { return m_marioState; }

void Mario::setMarioState(MarioState state) {
  m_marioState = state;
  rebuildFixture();
  if (m_sprite) {
    if (m_marioState == MarioState::FIRE) {
      m_sprite->setColor(sf::Color(255, 140, 0));
    } else {
      m_sprite->setColor(sf::Color::White);
    }
  }
}

void Mario::addScore(int points) {
  m_score += points;
}

int Mario::getScore() const {
  return m_score;
}

void Mario::addCoin() {
  m_coinCount++;
}

int Mario::getCoinCount() const {
  return m_coinCount;
}

std::unique_ptr<FireBall> Mario::shootFireBall(b2World* world) {
  if (!canShootFireBall() || !world || m_isDying || !m_active) {
    return nullptr;
  }

  float spawnX = m_position.x + (getFacingDirection() == Direction::RIGHT ? m_size.x + 4.f : -16.f);
  float spawnY = m_position.y + m_size.y * 0.4f;
  sf::Vector2f spawnPos(spawnX, spawnY);

  return std::make_unique<FireBall>(spawnPos, getFacingDirection(), world);
}

void Mario::setInvincible(float duration) {
  m_isInvincible = true;
  m_invincibilityTimer = duration;
}

void Mario::updateInvincibility(float dt) {
  if (!m_isInvincible) {
    if (m_sprite && m_marioState != MarioState::FIRE) {
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
      sf::Color baseColor = (m_marioState == MarioState::FIRE) ? sf::Color(255, 140, 0) : sf::Color::White;
      m_sprite->setColor(baseColor);
    }
  }

  if (m_invincibilityTimer <= 0.f) {
    m_isInvincible = false;
    m_invincibilityTimer = 0.f;
    if (m_sprite) {
      sf::Color baseColor = (m_marioState == MarioState::FIRE) ? sf::Color(255, 140, 0) : sf::Color::White;
      m_sprite->setColor(baseColor);
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

bool Mario::isDying() const {
  return m_isDying;
}

// ── Double Dispatch Collision Resolution ────────────────────────

void Mario::onCollisionBegin(Entity* other, b2Contact* contact, const b2Vec2& normal) {
  if (!other || m_isDying) return;

  b2WorldManifold worldManifold;
  contact->GetWorldManifold(&worldManifold);
  b2Vec2 contactNormal = normal;

  b2Vec2 marioVel = (m_body) ? m_body->GetLinearVelocity() : b2Vec2(0.f, 0.f);

  // 1. Collectible Item Pickup
  if (other->getType() == EntityType::ITEM) {
    Item* item = static_cast<Item*>(other);
    if (item && !item->isCollected()) {
      item->onCollect(*this);
    }
    return;
  }

  // 2. Enemy Interaction
  if (other->getType() == EntityType::ENEMY) {
    Enemy* enemy = static_cast<Enemy*>(other);
    // Stomp check
    if (contactNormal.y > TOP_STOMP_NORMAL_THRESHOLD && std::abs(contactNormal.x) < MAX_WALL_NORMAL_X && marioVel.y >= -0.1f) {
      setGrounded(true);
      if (enemy) {
        enemy->onStomp();
        EventBus::getInstance().notify(EventType::ENEMY_STOMPED);

        if (m_body) {
          float currentY = m_body->GetLinearVelocity().y;
          float bounceVel = -PhysicsEngine::pixelsToMeters(currentY > 0 ? STOMP_BOUNCE_SPEED : 200.f);
          m_body->SetLinearVelocity(b2Vec2(m_body->GetLinearVelocity().x, bounceVel));
        }
      }
    } else { // Lateral / Bottom Collision
      if (enemy) {
        enemy->onSideCollision(this);
      }
    }
    return;
  }

  // 3. Terrain / Ground Contact
  if (contactNormal.y > TOP_STOMP_NORMAL_THRESHOLD && std::abs(contactNormal.x) < MAX_WALL_NORMAL_X && marioVel.y >= -0.1f) {
    setGrounded(true);
  }
}

void Mario::onCollisionEnd(Entity* other, b2Contact* contact) {
  (void)other;
  (void)contact;
}