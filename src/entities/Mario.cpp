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
#include "entities/FireBall.h"
#include "entities/Enemy.h"
#include "items/Item.h"
#include "physics/PhysicsEngine.h"
#include "level/TileMap.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "core/AnimationSystem.h"
#include "states/SmallMarioState.h"
#include "states/SuperMarioState.h"
#include "states/FireMarioState.h"
#include "core/SpriteFrames.h"

namespace {
constexpr int DEFAULT_MARIO_HEALTH = 100;
constexpr int DEFAULT_MARIO_LIVES = 3;
constexpr float DEFAULT_JUMP_FORCE = 460.f;
constexpr float MAX_FALL_SPEED = 600.f;
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
constexpr float DAMAGE_INVINCIBILITY_DURATION = 1.0f;
constexpr float GROUND_NORMAL_Y_THRESHOLD = 0.8f;
constexpr float MAX_GROUND_NORMAL_X = 0.5f;

// Dimensions & Physics Constants
const sf::Vector2f DEFAULT_MARIO_POSITION(100.f, 100.f);
const sf::Vector2f SMALL_MARIO_SIZE(28.f, 30.f);
const sf::Vector2f SUPER_MARIO_SIZE(28.f, 60.f);
constexpr float MARIO_FIXTURE_DENSITY = 1.0f;
constexpr float MARIO_FIXTURE_FRICTION = 0.0f;

constexpr const char* MARIO_TEXTURE_PATH = "assets/textures/mario/MarioLuigi.png";

// Helper: register animation clips for the current MarioState and CharacterType
void setupAnimationsForState(AnimationSystem& animSys, MarioState state, CharacterType charType) {
    if (charType == CharacterType::LUIGI) {
        switch (state) {
        case MarioState::SMALL: {
            namespace F = SpriteFrames::SmallLuigi;
            animSys.addAnimation("idle",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::IDLE},  1.f));
            animSys.addAnimation("walk",  AnimationSystem::createManualAnimation(F::walkFrames(), 0.1f));
            animSys.addAnimation("jump",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::JUMP},  1.f));
            animSys.addAnimation("skid",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::SKID},  1.f));
            animSys.addAnimation("death", AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::DEATH}, 1.f, false));
            break;
        }
        case MarioState::SUPER: {
            namespace F = SpriteFrames::BigLuigi;
            animSys.addAnimation("idle",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::IDLE},  1.f));
            animSys.addAnimation("walk",  AnimationSystem::createManualAnimation(F::walkFrames(), 0.1f));
            animSys.addAnimation("jump",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::JUMP},  1.f));
            animSys.addAnimation("skid",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::SKID},  1.f));
            animSys.addAnimation("death", AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{SpriteFrames::SmallLuigi::DEATH}, 1.f, false));
            break;
        }
        case MarioState::FIRE: {
            namespace F = SpriteFrames::FireBigMario;
            animSys.addAnimation("idle",   AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::IDLE},   1.f));
            animSys.addAnimation("walk",   AnimationSystem::createManualAnimation(F::walkFrames(), 0.1f));
            animSys.addAnimation("jump",   AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::JUMP},   1.f));
            animSys.addAnimation("skid",   AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::SKID},   1.f));
            animSys.addAnimation("action", AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::ACTION}, 0.15f, false));
            animSys.addAnimation("death",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{SpriteFrames::SmallLuigi::DEATH}, 1.f, false));
            break;
        }
        case MarioState::FIRE_SMALL: {
            namespace F = SpriteFrames::FireSmallMario;
            animSys.addAnimation("idle",   AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::IDLE},   1.f));
            animSys.addAnimation("walk",   AnimationSystem::createManualAnimation(F::walkFrames(), 0.1f));
            animSys.addAnimation("jump",   AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::JUMP},   1.f));
            animSys.addAnimation("skid",   AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::SKID},   1.f));
            animSys.addAnimation("action", AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::JUMP},   0.15f, false));
            animSys.addAnimation("death",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::DEATH},  1.f, false));
            break;
        }
        }
    } else {
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
            animSys.addAnimation("idle",   AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::IDLE},   1.f));
            animSys.addAnimation("walk",   AnimationSystem::createManualAnimation(F::walkFrames(), 0.1f));
            animSys.addAnimation("jump",   AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::JUMP},   1.f));
            animSys.addAnimation("skid",   AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::SKID},   1.f));
            animSys.addAnimation("action", AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::ACTION}, 0.15f, false));
            animSys.addAnimation("death",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{SpriteFrames::SmallMario::DEATH}, 1.f, false));
            break;
        }
        case MarioState::FIRE_SMALL: {
            namespace F = SpriteFrames::FireSmallMario;
            animSys.addAnimation("idle",   AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::IDLE},   1.f));
            animSys.addAnimation("walk",   AnimationSystem::createManualAnimation(F::walkFrames(), 0.1f));
            animSys.addAnimation("jump",   AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::JUMP},   1.f));
            animSys.addAnimation("skid",   AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::SKID},   1.f));
            animSys.addAnimation("action", AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::JUMP},   0.15f, false));
            animSys.addAnimation("death",  AnimationSystem::createManualAnimation(std::vector<sf::IntRect>{F::DEATH},  1.f, false));
            break;
        }
        }
    }
}
} // namespace

Mario::Mario()
    : Character(DEFAULT_MARIO_POSITION, SMALL_MARIO_SIZE, DEFAULT_MARIO_HEALTH),
      m_marioState(MarioState::SMALL),
      m_characterType(CharacterType::LUIGI),
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
      m_transformTimer(0.f),
      m_pendingPowerDown(false),
      m_respawnPosition(DEFAULT_MARIO_POSITION) {
    setupAnimationsForState(*m_animationSystem, m_marioState, m_characterType);
    playAnimation("idle");
    setSprite(MARIO_TEXTURE_PATH);
}

Mario::Mario(const sf::Vector2f &position, const sf::Vector2f &size)
    : Character(position, size, DEFAULT_MARIO_HEALTH),
      m_marioState(MarioState::SMALL),
      m_characterType(CharacterType::LUIGI),
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
      m_transformTimer(0.f),
      m_pendingPowerDown(false),
      m_respawnPosition(position) {
    setupAnimationsForState(*m_animationSystem, m_marioState, m_characterType);
    playAnimation("idle");
    setSprite(MARIO_TEXTURE_PATH);
}

void Mario::update(float dt) {
  if (!m_active) return;

  if (m_pendingFixtureRebuild) {
    rebuildFixture();
  }

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
        m_sprite->setColor((m_marioState == MarioState::FIRE && m_characterType == CharacterType::MARIO) ? sf::Color(255, 140, 0) : sf::Color::White);
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
        respawn(m_respawnPosition);
        setInvincible(DAMAGE_INVINCIBILITY_DURATION);
      } else {
        takeDamage(FATAL_DAMAGE);
        m_active = false;
        EventBus::getInstance().notify(EventType::PLAYER_DIED);
      }
    }
    return;
  }

  // CRITICAL: Sync Box2D physics before doing custom movement/clamp logic
  syncPhysics();

  if (m_pendingPowerDown) {
    m_pendingPowerDown = false;
    powerDown();
  }

  // Tick invincibility & fire cooldown timers
  updateInvincibility(dt);
  if (m_fireCooldown > 0.0f) {
    m_fireCooldown -= dt;
    if (m_fireCooldown < 0.0f) {
      m_fireCooldown = 0.0f;
    }
  }

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
  updateInvincibility(dt);

  if (m_sprite) {
    sf::IntRect texRect = m_sprite->getTextureRect();
    if (texRect.size.x > 0 && texRect.size.y > 0) {
      float spriteWidth = static_cast<float>(texRect.size.x) * 2.f;
      float spriteHeight = static_cast<float>(texRect.size.y) * 2.f;
      float groundY = m_position.y + m_size.y;
      float spriteX = m_position.x + (m_size.x - spriteWidth) / 2.f;
      float spriteY = groundY - spriteHeight;

      m_sprite->setScale({2.f, 2.f});
      m_sprite->setPosition({spriteX, spriteY});
    }
  }

  // Left world boundary clamp (S6-TV3-08): prevent Mario from moving left off-screen past X = 0
  constexpr float MIN_WORLD_X = 16.0f; // Half Mario's width (32/2)
  if (m_body) {
    b2Vec2 bodyPos = m_body->GetPosition();
    float minXPosMeters = PhysicsEngine::pixelsToMeters(MIN_WORLD_X);
    if (bodyPos.x < minXPosMeters) {
      m_body->SetTransform(b2Vec2(minXPosMeters, bodyPos.y), m_body->GetAngle());
      b2Vec2 vel = m_body->GetLinearVelocity();
      if (vel.x < 0.0f) {
        m_body->SetLinearVelocity(b2Vec2(0.0f, vel.y));
      }
    }
  }

  // Dynamic pit fall check (S6-TV3-07): uses dynamic m_pitThreshold set from Level map bounds
  if (m_position.y > m_pitThreshold) {
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
  applyMovementPhysics(dt, m_inputDirX, m_isRunning, m_jumpRequested, m_jumpReleased);
  m_jumpRequested = false;
  m_jumpReleased = false;
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

void Mario::initPhysics(b2World* world, b2BodyType type, const sf::Vector2f& size, bool isSensor) {
  Entity::initPhysics(world, type, size, isSensor);
  rebuildFixture();
}

void Mario::rebuildFixture() {
  sf::Vector2f targetSize = (m_marioState == MarioState::SMALL || m_marioState == MarioState::FIRE_SMALL)
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
  // Use a beveled octagon (chamfered box) to prevent Box2D ghost collisions on seams between blocks
  b2PolygonShape dynamicBox;
  float w = PhysicsEngine::pixelsToMeters(targetSize.x / 2.0f);
  float h = PhysicsEngine::pixelsToMeters(targetSize.y / 2.0f);
  float bevel = PhysicsEngine::pixelsToMeters(4.0f); // 4-pixel corner bevel

  b2Vec2 vertices[8];
  vertices[0].Set(-w + bevel, -h); // Top edge, left
  vertices[1].Set( w - bevel, -h); // Top edge, right
  vertices[2].Set( w, -h + bevel); // Right edge, top
  vertices[3].Set( w,  h - bevel); // Right edge, bottom
  vertices[4].Set( w - bevel,  h); // Bottom edge, right
  vertices[5].Set(-w + bevel,  h); // Bottom edge, left
  vertices[6].Set(-w,  h - bevel); // Left edge, bottom
  vertices[7].Set(-w, -h + bevel); // Left edge, top
  
  dynamicBox.Set(vertices, 8);

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

  m_jumpRequested = true;
}

void Mario::releaseJump() {
  m_jumpReleased = true;
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

bool Mario::hasCeilingClearance() const {
  if (!m_body || !m_body->GetWorld()) return true;

  b2AABB aabb;
  b2Vec2 pos = m_body->GetPosition();
  float halfW = PhysicsEngine::pixelsToMeters(14.0f);
  float currentTop = pos.y - PhysicsEngine::pixelsToMeters(16.0f);
  float requiredTop = pos.y - PhysicsEngine::pixelsToMeters(32.0f);

  aabb.lowerBound.Set(pos.x - halfW, requiredTop);
  aabb.upperBound.Set(pos.x + halfW, currentTop);

  class ClearanceQueryCallback : public b2QueryCallback {
  public:
    bool hasCollision = false;
    b2Body* marioBody;
    ClearanceQueryCallback(b2Body* body) : marioBody(body) {}

    bool ReportFixture(b2Fixture* fixture) override {
      b2Body* body = fixture->GetBody();
      if (body == marioBody || fixture->IsSensor()) return true;
      hasCollision = true;
      return false; // Stop query on first solid collision
    }
  } callback(m_body);

  m_body->GetWorld()->QueryAABB(&callback, aabb);
  return !callback.hasCollision;
}

void Mario::powerUp(MarioState state) {
  if (m_marioState == state) return;

  bool wasSmall = (m_marioState == MarioState::SMALL || m_marioState == MarioState::FIRE_SMALL);
  m_marioState = state;

  if (state == MarioState::SMALL) {
    m_statePattern = std::make_unique<SmallMarioState>();
  } else if (state == MarioState::SUPER) {
    m_statePattern = std::make_unique<SuperMarioState>();
  } else if (state == MarioState::FIRE || state == MarioState::FIRE_SMALL) {
    m_statePattern = std::make_unique<FireMarioState>();
  }

  // If growing from Small to Super/Fire, offset Box2D body upward by 16px (0.5m)
  // ONLY if overhead clearance exists, otherwise anchor to current position to avoid geometry clipping
  if (wasSmall && (state == MarioState::SUPER || state == MarioState::FIRE) && m_body) {
    if (m_body->GetWorld() && !m_body->GetWorld()->IsLocked() && hasCeilingClearance()) {
      b2Vec2 currentPos = m_body->GetPosition();
      m_body->SetTransform(b2Vec2(currentPos.x, currentPos.y - PhysicsEngine::pixelsToMeters(16.f)), m_body->GetAngle());
    }
  }

  setupAnimationsForState(*m_animationSystem, m_marioState, m_characterType);
  playAnimation("idle");

  EventBus::getInstance().notify(EventType::PLAYER_POWER_UP);
  rebuildFixture();

  if (m_sprite) {
    m_sprite->setColor(sf::Color::White);
  }
}

void Mario::setCharacterType(CharacterType type) {
  if (m_characterType == type) return;
  m_characterType = type;
  setupAnimationsForState(*m_animationSystem, m_marioState, m_characterType);
  playAnimation("idle");
}


void Mario::powerDown() {
  if (m_isInvincible || m_isDying || m_isTransforming) {
    return;
  }
  if (m_body && m_body->GetWorld() && m_body->GetWorld()->IsLocked()) {
    queuePowerDown();
    return;
  }

  if (m_marioState == MarioState::FIRE) {
    powerUp(MarioState::SUPER);
    rebuildFixture();
    setInvincible(DAMAGE_INVINCIBILITY_DURATION);
    EventBus::getInstance().notify(EventType::PLAYER_POWER_DOWN);
  } else if (m_marioState == MarioState::FIRE_SMALL) {
    powerUp(MarioState::SMALL);
    rebuildFixture();
    setInvincible(DAMAGE_INVINCIBILITY_DURATION);
    EventBus::getInstance().notify(EventType::PLAYER_POWER_DOWN);
  } else if (m_marioState == MarioState::SUPER) {
    powerUp(MarioState::SMALL);
    rebuildFixture();
    setInvincible(DAMAGE_INVINCIBILITY_DURATION);
    EventBus::getInstance().notify(EventType::PLAYER_POWER_DOWN);
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
  if (m_isDying) return;

  m_isDying = true;
  m_deathTimer = 0.5f;

  if (m_lives > 0) {
    m_lives--;
  }

  // Reset to SMALL state and reload animations for character type immediately
  m_marioState = MarioState::SMALL;
  setupAnimationsForState(*m_animationSystem, m_marioState, m_characterType);
  playAnimation("death");

  if (m_sprite) {
    m_sprite->setColor(sf::Color::White);
  }

  if (m_body) {
    m_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
  }

#ifdef DEBUG
  std::cout << "[DEBUG][Mario] Mario died. Lives remaining: " << m_lives << std::endl;
#endif

  // S6-TV1-18: PLAYER_DIED fires on EVERY death (life already decremented exactly
  // once above). Whether this results in a level reload (non-terminal death) or a
  // GameOver is decided by the observer (PlayState) from the remaining lives.
  EventBus::getInstance().notify(EventType::PLAYER_DIED);

  if (m_lives > 0) {
    m_active = false;
    EventBus::getInstance().notify(EventType::PLAYER_LOST_LIFE);
  } else {
    // Game over death
    takeDamage(FATAL_DAMAGE);
    m_active = false;
  }
}

void Mario::respawn(const sf::Vector2f& spawnPosition) {
  m_marioState = MarioState::SMALL;
  m_size = SMALL_MARIO_SIZE;
  m_health = DEFAULT_MARIO_HEALTH;
  m_active = true;
  m_isDying = false;
  clearGroundedState();
  setPosition(spawnPosition);

  if (m_body) {
    m_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
    m_body->SetAngularVelocity(0.f);
    m_body->SetAwake(true);
  }
  setupAnimationsForState(*m_animationSystem, m_marioState, m_characterType);
  playAnimation("idle");
  rebuildFixture();

  if (m_sprite) {
    m_sprite->setColor(sf::Color::White);
  }
}

void Mario::setRespawnPosition(const sf::Vector2f& spawnPosition) {
  m_respawnPosition = spawnPosition;
}

MarioState Mario::getMarioState() const { return m_marioState; }

void Mario::setMarioState(MarioState state) {
  m_marioState = state;
  // S6-TV1-10: rebuilding just the fixture leaves the previous power state's
  // animation clips registered. Rebuild the clips too so an SMALL/SUPER/FIRE
  // restore from GameProgress uses the correct sprites.
  setupAnimationsForState(*m_animationSystem, m_marioState, m_characterType);
  playAnimation("idle");
  rebuildFixture();
  if (m_sprite) {
    if (m_marioState == MarioState::FIRE && m_characterType == CharacterType::MARIO) {
      m_sprite->setColor(sf::Color(255, 140, 0));
    } else {
      m_sprite->setColor(sf::Color::White);
    }
  }
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

void Mario::setScore(int score) {
  m_score = score;
}

void Mario::setCoinCount(int coins) {
  m_coinCount = coins;
}

std::unique_ptr<FireBall> Mario::shootFireBall(b2World* world) {
  if (!canShootFireBall() || !world || m_isDying || !m_active) {
    return nullptr;
  }

  m_fireCooldown = FIRE_COOLDOWN_DURATION;
  playAnimation("action");

  float spawnX = m_position.x + (getFacingDirection() == Direction::RIGHT ? m_size.x + 4.f : -16.f);
  float spawnY = m_position.y + 4.f;
  sf::Vector2f spawnPos(spawnX, spawnY);

  return std::make_unique<FireBall>(spawnPos, getFacingDirection(), world);
}

void Mario::setInvincible(float duration) {
  m_isInvincible = true;
  m_invincibilityTimer = duration;
}

void Mario::setStarInvincible(float duration) {
  m_isStarInvincible = true;
  m_starInvincibilityTimer = duration;
  m_isInvincible = true;
}

bool Mario::isStarInvincible() const {
  return m_isStarInvincible;
}

void Mario::updateInvincibility(float dt) {
  // 1. Starman Rainbow Invincibility (100% opaque, 6-color NES cycle)
  if (m_isStarInvincible) {
    m_starInvincibilityTimer -= dt;
    if (m_sprite) {
      static const sf::Color rainbow[] = {
        sf::Color(255, 60, 60),   // Bright Red
        sf::Color(255, 200, 40),  // Bright Yellow
        sf::Color(60, 255, 60),   // Bright Green
        sf::Color(60, 220, 255),  // Bright Cyan
        sf::Color(255, 100, 255), // Bright Magenta
        sf::Color::White
      };
      int colorIdx = static_cast<int>((10.f - m_starInvincibilityTimer) * 15.f) % 6;
      if (colorIdx < 0) colorIdx = 0;
      m_sprite->setColor(rainbow[colorIdx]);
    }

    if (m_starInvincibilityTimer <= 0.f) {
      m_isStarInvincible = false;
      m_starInvincibilityTimer = 0.f;
      m_isInvincible = false;
      if (m_sprite) {
        sf::Color baseColor = (m_marioState == MarioState::FIRE) ? sf::Color(255, 140, 0) : sf::Color::White;
        m_sprite->setColor(baseColor);
      }
    }
    return;
  }

  // 2. Damage Invincibility (short blink)
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
  return m_isInvincible || m_isStarInvincible;
}

bool Mario::canShootFireBall() const {
  return (m_marioState == MarioState::FIRE || m_marioState == MarioState::FIRE_SMALL) && m_fireCooldown <= 0.0f;
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
      item->markForRemoval();
    }
    return;
  }

  // 2. Enemy Interaction
  if (other->getType() == EntityType::ENEMY) {
    Enemy* enemy = static_cast<Enemy*>(other);
    // Stomp check
    bool isStomp = false;
    if (contactNormal.y > TOP_STOMP_NORMAL_THRESHOLD && std::abs(contactNormal.x) < MAX_WALL_NORMAL_X) {
      isStomp = true;
    } else {
      b2Body* enemyBody = other->getBody();
      if (enemyBody && m_body) {
        float marioHalfHeight = PhysicsEngine::pixelsToMeters(getSize().y / 2.0f);
        float marioBottomMeters = m_body->GetPosition().y + marioHalfHeight;
        float enemyMidMeters = enemyBody->GetPosition().y;
        float tolerance = PhysicsEngine::pixelsToMeters(other->getSize().y * 0.2f);
        if (marioBottomMeters <= enemyMidMeters + tolerance) {
          isStomp = true;
        }
      }
    }

    if (isStomp) {
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
