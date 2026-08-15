/**
 * @file Mario.cpp
 * @author TV3 (Bảo)
 * @brief Mario character implementation with authentic NES movement physics and
 * states
 * @note Sprint 6 TV3: foot-anchored growth, low-ceiling query, independent
 * invincibility clocks, death/respawn lifecycle, fixture bounds
 */

#include "entities/Mario.h"
#include "core/AnimationSystem.h"
#include "core/ScoreRules.h"
#include "core/SpriteFrames_shared.h"
#include "level/TileMap.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "physics/PhysicsEngine.h"
#include "states/SmallFireMarioState.h"
#include "states/SmallMarioState.h"
#include "states/SuperFireMarioState.h"
#include "states/SuperMarioState.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>

namespace {
constexpr int DEFAULT_MARIO_HEALTH = 100;
constexpr int DEFAULT_MARIO_LIVES = 3;
constexpr float MAX_FALL_SPEED = 600.f;

// The shared atlas exposes this Mario-style progression. SMB1 also has a
// 500-point step, but there is no SCORE_500 frame in SpriteFrames_shared.h.
constexpr std::array<int, 9> STOMP_SCORE_CHAIN = {
    100, 200, 400, 800, 1000, 2000, 4000, 5000, 8000
};

// Authentic Mario Movement Physics Constants (in pixels/sec)
// Build speed progressively so Mario does not reach the movement cap almost
// immediately after the direction key is pressed.
constexpr float GROUND_ACCEL = 220.f;
constexpr float RUN_ACCEL = 400.f;
// Lower friction preserves more horizontal momentum when the input is
// released or reversed, giving Mario a slightly longer, harder-to-control
// slide.
constexpr float GROUND_FRICTION = 425.f;
constexpr float SKID_FRICTION = 575.f;
constexpr float AIR_ACCEL = 125.f;
constexpr float AIR_FRICTION = 300.f;
constexpr float SHORT_HOP_CUTOFF = 0.5f;
constexpr float SKID_SPEED_THRESHOLD = 15.0f;
constexpr float ASCENDING_VEL_THRESHOLD = -0.5f;
// Give Mario a little more room to recover after shrinking. During this
// window damaging enemies are intangible to him in CollisionManager.
constexpr float DAMAGE_INVINCIBILITY_DURATION = 1.5f;
constexpr float DEATH_ANIMATION_DURATION = 0.5f;
constexpr float DEATH_POST_ANIMATION_FALL_DELAY = 0.4f;
constexpr float DEATH_JUMP_SPEED = 320.f;
constexpr float SPAWN_ANIMATION_DURATION = 0.5f;
constexpr float TRANSFORM_PRESENTATION_DURATION = 0.5f;
constexpr float TRANSFORM_FRAME_DURATION = 0.07f;
constexpr float GROUND_NORMAL_Y_THRESHOLD = 0.8f;
constexpr float MAX_GROUND_NORMAL_X = 0.5f;
// Underwater swim physics constants
constexpr float UNDERWATER_SWIM_IMPULSE =
    -5.5f; // upward impulse per swim stroke (in m/s)
constexpr float UNDERWATER_MAX_SINK_SPEED =
    3.0f; // max downward velocity (in m/s)
constexpr float UNDERWATER_ACCEL = 400.f;
constexpr float UNDERWATER_FRICTION = 600.f;

// Dimensions & Physics Constants
const sf::Vector2f DEFAULT_MARIO_POSITION(100.f, 100.f);
const sf::Vector2f SMALL_MARIO_SIZE(28.f, 30.f);
const sf::Vector2f SUPER_MARIO_SIZE(28.f, 60.f);
constexpr float MARIO_FIXTURE_DENSITY = 1.0f;
constexpr float MARIO_FIXTURE_FRICTION = 0.0f;

constexpr const char *MARIO_TEXTURE_PATH =
    "assets/textures/mario/MarioLuigi.png";

bool usesSuperBody(MarioState state) {
  return state == MarioState::SUPER || state == MarioState::FIRE_SUPER;
}

bool usesFire(MarioState state) {
  return state == MarioState::FIRE_SMALL || state == MarioState::FIRE_SUPER;
}

sf::Vector2f bodySizeForState(MarioState state) {
  return usesSuperBody(state) ? SUPER_MARIO_SIZE : SMALL_MARIO_SIZE;
}

// Helper: register animation clips for the current MarioState and
// CharacterType. Fire body size is encoded directly in the enum.
void setupAnimationsForState(AnimationSystem &animSys, MarioState state,
                             CharacterType charType) {
  auto addSwimAndClimb = [&animSys](const auto &climbFrames,
                                    const auto &swimFrames) {
    animSys.addAnimation(
        "climb", AnimationSystem::createManualAnimation(climbFrames, 0.12f));
    animSys.addAnimation(
        "climb_up", AnimationSystem::createManualAnimation(climbFrames, 0.12f));

    std::vector<sf::IntRect> climbDownFrames(climbFrames.rbegin(),
                                             climbFrames.rend());
    animSys.addAnimation("climb_down", AnimationSystem::createManualAnimation(
                                           climbDownFrames, 0.12f));
    animSys.addAnimation(
        "climb_idle", AnimationSystem::createManualAnimation(
                          std::vector<sf::IntRect>{climbFrames.front()}, 1.0f));

    animSys.addAnimation(
        "swim", AnimationSystem::createManualAnimation(swimFrames, 0.10f));
  };
  auto addFireAnimations = [&](const sf::IntRect &idle,
                               const std::vector<sf::IntRect> &walk,
                               const std::vector<sf::IntRect> &climb,
                               const std::vector<sf::IntRect> &swim,
                               const sf::IntRect &jump, const sf::IntRect &skid,
                               const std::vector<sf::IntRect> &action,
                               const sf::IntRect &death) {
    animSys.addAnimation("idle", AnimationSystem::createManualAnimation(
                                     std::vector<sf::IntRect>{idle}, 1.f));
    animSys.addAnimation("walk",
                         AnimationSystem::createManualAnimation(walk, 0.1f));
    addSwimAndClimb(climb, swim);
    animSys.addAnimation("jump", AnimationSystem::createManualAnimation(
                                     std::vector<sf::IntRect>{jump}, 1.f));
    animSys.addAnimation("skid", AnimationSystem::createManualAnimation(
                                     std::vector<sf::IntRect>{skid}, 1.f));
    animSys.addAnimation(
        "action", AnimationSystem::createManualAnimation(action, 0.15f, false));
    animSys.addAnimation("death", AnimationSystem::createManualAnimation(
                                      std::vector<sf::IntRect>{death},
                                      DEATH_ANIMATION_DURATION, false));
    animSys.addAnimation("spawn", AnimationSystem::createManualAnimation(
                                      std::vector<sf::IntRect>{idle},
                                      SPAWN_ANIMATION_DURATION, false));
  };
  switch (state) {
  case MarioState::SMALL: {
    if (charType == CharacterType::LUIGI) {
      namespace F = SpriteFrames::shared::SmallLuigi;
      animSys.addAnimation("idle", AnimationSystem::createManualAnimation(
                                       std::vector<sf::IntRect>{F::IDLE}, 1.f));
      animSys.addAnimation("walk", AnimationSystem::createManualAnimation(
                                       F::walkFrames(), 0.1f));
      addSwimAndClimb(F::climbFrames(), F::swimFrames());
      animSys.addAnimation("jump", AnimationSystem::createManualAnimation(
                                       std::vector<sf::IntRect>{F::JUMP}, 1.f));
      animSys.addAnimation("skid", AnimationSystem::createManualAnimation(
                                       std::vector<sf::IntRect>{F::SKID}, 1.f));
      animSys.addAnimation("death", AnimationSystem::createManualAnimation(
                                        std::vector<sf::IntRect>{F::DEATH},
                                        DEATH_ANIMATION_DURATION, false));
      animSys.addAnimation("spawn", AnimationSystem::createManualAnimation(
                                        std::vector<sf::IntRect>{F::IDLE},
                                        SPAWN_ANIMATION_DURATION, false));
    } else {
      namespace F = SpriteFrames::shared::SmallMario;
      animSys.addAnimation("idle", AnimationSystem::createManualAnimation(
                                       std::vector<sf::IntRect>{F::IDLE}, 1.f));
      animSys.addAnimation("walk", AnimationSystem::createManualAnimation(
                                       F::walkFrames(), 0.1f));
      addSwimAndClimb(F::climbFrames(), F::swimFrames());
      animSys.addAnimation("jump", AnimationSystem::createManualAnimation(
                                       std::vector<sf::IntRect>{F::JUMP}, 1.f));
      animSys.addAnimation("skid", AnimationSystem::createManualAnimation(
                                       std::vector<sf::IntRect>{F::SKID}, 1.f));
      animSys.addAnimation("death", AnimationSystem::createManualAnimation(
                                        std::vector<sf::IntRect>{F::DEATH},
                                        DEATH_ANIMATION_DURATION, false));
      animSys.addAnimation("spawn", AnimationSystem::createManualAnimation(
                                        std::vector<sf::IntRect>{F::IDLE},
                                        SPAWN_ANIMATION_DURATION, false));
    }
    break;
  }
  case MarioState::SUPER: {
    if (charType == CharacterType::LUIGI) {
      namespace F = SpriteFrames::shared::BigLuigi;
      animSys.addAnimation("idle", AnimationSystem::createManualAnimation(
                                       std::vector<sf::IntRect>{F::IDLE}, 1.f));
      animSys.addAnimation("walk", AnimationSystem::createManualAnimation(
                                       F::walkFrames(), 0.1f));
      addSwimAndClimb(F::climbFrames(), F::swimFrames());
      animSys.addAnimation("jump", AnimationSystem::createManualAnimation(
                                       std::vector<sf::IntRect>{F::JUMP}, 1.f));
      animSys.addAnimation("crouch", AnimationSystem::createManualAnimation(
                                         std::vector<sf::IntRect>{F::CROUCH}, 1.f));
      animSys.addAnimation("skid", AnimationSystem::createManualAnimation(
                                       std::vector<sf::IntRect>{F::SKID}, 1.f));
      animSys.addAnimation(
          "death",
          AnimationSystem::createManualAnimation(
              std::vector<sf::IntRect>{SpriteFrames::shared::SmallLuigi::DEATH},
              DEATH_ANIMATION_DURATION, false));
      animSys.addAnimation("spawn", AnimationSystem::createManualAnimation(
                                        std::vector<sf::IntRect>{F::IDLE},
                                        SPAWN_ANIMATION_DURATION, false));
    } else {
      namespace F = SpriteFrames::shared::BigMario;
      animSys.addAnimation("idle", AnimationSystem::createManualAnimation(
                                       std::vector<sf::IntRect>{F::IDLE}, 1.f));
      animSys.addAnimation("walk", AnimationSystem::createManualAnimation(
                                       F::walkFrames(), 0.1f));
      addSwimAndClimb(F::climbFrames(), F::swimFrames());
      animSys.addAnimation("jump", AnimationSystem::createManualAnimation(
                                       std::vector<sf::IntRect>{F::JUMP}, 1.f));
      animSys.addAnimation("crouch", AnimationSystem::createManualAnimation(
                                         std::vector<sf::IntRect>{F::CROUCH}, 1.f));
      animSys.addAnimation("skid", AnimationSystem::createManualAnimation(
                                       std::vector<sf::IntRect>{F::SKID}, 1.f));
      animSys.addAnimation(
          "death",
          AnimationSystem::createManualAnimation(
              std::vector<sf::IntRect>{SpriteFrames::shared::SmallMario::DEATH},
              DEATH_ANIMATION_DURATION, false));
      animSys.addAnimation("spawn", AnimationSystem::createManualAnimation(
                                        std::vector<sf::IntRect>{F::IDLE},
                                        SPAWN_ANIMATION_DURATION, false));
    }
    break;
  }
  case MarioState::FIRE_SMALL: {
    namespace F = SpriteFrames::shared::FireSmallMario;
    addFireAnimations(
        F::IDLE, F::walkFrames(), F::climbFrames(), F::swimFrames(), F::JUMP,
        F::SKID,
        std::vector<sf::IntRect>{F::WALK3},
        F::DEATH);
    break;
  }
  case MarioState::FIRE_SUPER: {
    namespace F = SpriteFrames::shared::FireBigMario;
    addFireAnimations(
        F::IDLE, F::walkFrames(), F::climbFrames(), F::swimFrames(), F::JUMP,
        F::SKID,
        std::vector<sf::IntRect>{F::ACTION},
        charType == CharacterType::LUIGI
            ? SpriteFrames::shared::SmallLuigi::DEATH
            : SpriteFrames::shared::SmallMario::DEATH);
    animSys.addAnimation("crouch", AnimationSystem::createManualAnimation(
                                       std::vector<sf::IntRect>{F::CROUCH}, 1.f));
    break;
  }
  }
}
} // namespace

Mario::Mario()
    : Character(DEFAULT_MARIO_POSITION, SMALL_MARIO_SIZE, DEFAULT_MARIO_HEALTH),
      m_marioState(MarioState::SMALL), m_characterType(CharacterType::MARIO),
      m_statePattern(std::make_unique<SmallMarioState>()),
      m_jumpForce(characterProfileFor(CharacterType::MARIO).jumpForce),
      m_score(0), m_coinCount(0),
      m_isInvincible(false), m_invincibilityTimer(0.f),
      m_lives(DEFAULT_MARIO_LIVES), m_isDying(false), m_isSpawning(false),
      m_isRunning(false), m_isSkidding(false), m_isTransforming(false),
      m_transformTimer(0.f), m_pendingPowerDown(false),
      m_respawnPosition(DEFAULT_MARIO_POSITION) {
  setupAnimationsForState(*m_animationSystem, m_marioState, m_characterType);
  playAnimation("idle");
  setSprite(MARIO_TEXTURE_PATH);
}

Mario::Mario(const sf::Vector2f &position, const sf::Vector2f &size)
    : Character(position, size, DEFAULT_MARIO_HEALTH),
      m_marioState(MarioState::SMALL), m_characterType(CharacterType::MARIO),
      m_statePattern(std::make_unique<SmallMarioState>()),
      m_jumpForce(characterProfileFor(CharacterType::MARIO).jumpForce),
      m_score(0), m_coinCount(0),
      m_isInvincible(false), m_invincibilityTimer(0.f),
      m_lives(DEFAULT_MARIO_LIVES), m_isDying(false), m_isSpawning(false),
      m_isRunning(false), m_isSkidding(false), m_isTransforming(false),
      m_transformTimer(0.f), m_pendingPowerDown(false),
      m_respawnPosition(position) {
  setupAnimationsForState(*m_animationSystem, m_marioState, m_characterType);
  playAnimation("idle");
  setSprite(MARIO_TEXTURE_PATH);
}

bool Mario::handleDeathPhase(float dt) {
  if (!m_isDying) {
    return false;
  }

  if (m_animationSystem->getCurrentAnimationName() != "death") {
    playAnimation("death");
    updateAnimation(0.f);
  }
  if (m_body) {
    // Keep the characteristic death jump while suppressing horizontal
    // movement. The body is already sensor-only, so gravity can animate the
    // arc without letting Mario interact with the level again.
    const b2Vec2 velocity = m_body->GetLinearVelocity();
    m_body->SetLinearVelocity(b2Vec2(0.f, velocity.y));
  }
  syncPhysics();
  updateAnimation(dt);
  updateSpriteLayout();
  if (m_sprite) {
    m_sprite->setColor(sf::Color::White);
  }
  if (m_animationSystem->isFinished()) {
    // The sprite clip ends at the jump's apex. Keep gravity running for a
    // short tail so Mario visibly falls instead of freezing in mid-air.
    m_deathFallDelayTimer = std::max(0.0f, m_deathFallDelayTimer - dt);
    if (m_deathFallDelayTimer <= 0.0f) {
      m_deathAnimationFinished = true;
      // Keep the final DEATH frame drawable and freeze only after the fall
      // tail has completed and the state transition can consume the signal.
      if (m_body) {
        m_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
      }
    }
  }
  return true;
}

bool Mario::handleTransformPhase(float dt) {
  if (!m_isTransforming) {
    return false;
  }

  m_transformTimer -= dt;
  if (m_body) {
    b2Vec2 currentVel = m_body->GetLinearVelocity();
    m_body->SetLinearVelocity(b2Vec2(0.f, currentVel.y));
  }
  if (m_sprite) {
    int ms = static_cast<int>(m_transformTimer * 1000);
    m_sprite->setColor((ms / 50) % 2 == 0 ? sf::Color::Transparent
                                          : sf::Color::White);
  }
  syncPhysics();
  updateAnimation(dt);
  updateSpriteLayout();
  if (m_transformTimer <= 0.f) {
    m_isTransforming = false;
    m_transformTimer = 0.f;
    playAnimation("idle");
    updateAnimation(0.f);
    updateSpriteLayout();
    if (m_sprite && !m_isInvincible && !m_isStarInvincible) {
      m_sprite->setColor(sf::Color::White);
    }
  }
  return true;
}

bool Mario::handleSpawnPhase(float dt) {
  if (!m_isSpawning) {
    return false;
  }

  if (m_animationSystem->isFinished()) {
    m_isSpawning = false;
    playAnimation("idle");
    return false;
  }

  updateAnimation(dt);
  return true;
}

void Mario::updateMovementAnimations(float dt) {
  if (m_animationSystem->getCurrentAnimationName() == "action" &&
      !m_animationSystem->isFinished()) {
    // Keep playing action animation until it finishes so fireball throw is visible
  } else if (m_isFlagpoleSliding) {
    playAnimation("climb");
  } else if (m_isClimbing) {
    if (m_verticalIntent < 0.0f) {
      playAnimation("climb_up");
    } else if (m_verticalIntent > 0.0f) {
      playAnimation("climb_down");
    } else {
      playAnimation("climb_idle");
    }
  } else if (m_isUnderwater) {
    playAnimation("swim");
  } else if (!isGrounded()) {
    playAnimation("jump");
  } else if (m_verticalIntent > 0.5f &&
             (m_marioState == MarioState::SUPER ||
              m_marioState == MarioState::FIRE_SUPER)) {
    playAnimation("crouch");
  } else if (m_isSkidding) {
    playAnimation("skid");
  } else if (std::abs(getVelocity().x) > 5.f) {
    playAnimation("walk");
  } else {
    playAnimation("idle");
  }
  updateAnimation(dt);
  updateSpriteLayout();
}

void Mario::applyWorldBoundsClamp() {
  constexpr float MIN_WORLD_X = 16.0f; // Half Mario's width (32/2)
  if (m_body) {
    b2Vec2 bodyPos = m_body->GetPosition();
    float minXPosMeters = PhysicsEngine::pixelsToMeters(MIN_WORLD_X);
    if (bodyPos.x < minXPosMeters) {
      m_body->SetTransform(b2Vec2(minXPosMeters, bodyPos.y),
                           m_body->GetAngle());
      b2Vec2 vel = m_body->GetLinearVelocity();
      if (vel.x < 0.0f) {
        m_body->SetLinearVelocity(b2Vec2(0.0f, vel.y));
      }
    }
  }

  if (m_position.y > m_pitThreshold) {
    loseLife();
  }
}

void Mario::update(float dt) {
  if (!m_active)
    return;

  // CRITICAL (AGENTS.md #3): Unconditionally sync Box2D physics before movement logic
  syncPhysics();

  if (handleDeathPhase(dt)) {
    return;
  }

  updateInvincibility(dt);
  if (m_fireCooldown > 0.0f) {
    m_fireCooldown = std::max(0.0f, m_fireCooldown - dt);
  }

  if (m_pendingGrowthState != MarioState::SMALL) {
    const bool worldLocked =
        m_body && m_body->GetWorld() && m_body->GetWorld()->IsLocked();
    if (!worldLocked && hasGrowthClearance()) {
      MarioState target = m_pendingGrowthState;
      const bool withPresentation = m_pendingGrowthPresentation;
      m_pendingGrowthState = MarioState::SMALL;
      m_pendingGrowthPresentation = false;
      applyStateTransition(target, withPresentation);
    }
  }

  if (m_pendingFixtureRebuild) {
    rebuildFixture();
  }

  if (handleTransformPhase(dt)) {
    return;
  }

  if (handleSpawnPhase(dt)) {
    return;
  }

  if (m_pendingPowerDown) {
    m_pendingPowerDown = false;
    powerDown();
  }

  // Clamp terminal fall velocity to prevent AABB tunneling (TV3)
  if (m_body) {
    b2Vec2 velocity = m_body->GetLinearVelocity();
    float maxFallMeters = PhysicsEngine::pixelsToMeters(MAX_FALL_SPEED);
    if (velocity.y > maxFallMeters) {
      m_body->SetLinearVelocity(b2Vec2(velocity.x, maxFallMeters));
    }
  }

  updateMovementAnimations(dt);
  applyWorldBoundsClamp();
}

void Mario::preparePhysics(float dt) {
  if (!m_active || !m_body || m_isDying || m_isSpawning || m_isTransforming ||
      m_isFlagpoleSliding) {
    if (m_body && m_isDying) {
      // Keep the upward death impulse alive until Box2D advances. The old
      // zeroing here erased the jump set by loseLife() every frame.
      const b2Vec2 velocity = m_body->GetLinearVelocity();
      m_body->SetLinearVelocity(b2Vec2(0.f, velocity.y));
    } else if (m_body && (m_isSpawning || m_isTransforming)) {
      m_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
    }
    m_jumpRequested = false;
    m_jumpReleased = false;
    return;
  }

  // Apply player input before Box2D advances so the current frame reacts
  // immediately.
  applyMovementPhysics(dt, m_inputDirX, m_isRunning, m_jumpRequested,
                       m_jumpReleased);

  if (m_automaticWalkSpeed > 0.0f && m_body) {
    b2Vec2 velocity = m_body->GetLinearVelocity();
    const float maxSpeed = PhysicsEngine::pixelsToMeters(m_automaticWalkSpeed);
    velocity.x = std::clamp(velocity.x, -maxSpeed, maxSpeed);
    m_body->SetLinearVelocity(velocity);
  }

  m_jumpRequested = false;
  m_jumpReleased = false;
}

void Mario::applyGroundPhysics(float dt, float inputDirX, bool isRunningInput,
                               bool jumpKeyPressed, float &currentVy,
                               float &newVx, float targetMaxSpeed) {
  if (inputDirX != 0.0f) {
    if ((newVx > SKID_SPEED_THRESHOLD && inputDirX < 0.0f) ||
        (newVx < -SKID_SPEED_THRESHOLD && inputDirX > 0.0f)) {
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

      if (newVx > targetMaxSpeed)
        newVx = targetMaxSpeed;
      if (newVx < -targetMaxSpeed)
        newVx = -targetMaxSpeed;
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
    std::cout << "[DEBUG][Mario] Jump executed with velocity: "
              << jumpVelocityMeters << std::endl;
#endif

    EventBus::getInstance().notify(EventType::PLAYER_JUMPED);
  }
}

void Mario::applyAirPhysics(float dt, float inputDirX, bool jumpKeyReleased,
                            float &currentVy, float &newVx,
                            float targetMaxSpeed) {
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

void Mario::applyMovementPhysics(float dt, float inputDirX, bool isRunningInput,
                                 bool jumpKeyPressed, bool jumpKeyReleased) {
  if (!m_body || m_isDying)
    return;

  if (m_isFlagpoleSliding) {
    // Level::updateFlagpoleSlide owns the deterministic slide velocity.
    return;
  }

  if (m_isClimbing) {
    const float climbSpeed = 96.0f;
    m_body->SetLinearVelocity(b2Vec2(
        0.0f, PhysicsEngine::pixelsToMeters(m_verticalIntent * climbSpeed)));
    return;
  }

  b2Vec2 currentVelMeters = m_body->GetLinearVelocity();
  float currentVx = PhysicsEngine::metersToPixels(currentVelMeters.x);
  float currentVy = currentVelMeters.y;

  m_isRunning = isRunningInput;
  const CharacterProfile profile = characterProfileFor(m_characterType);

  // Underwater: override speeds and allow swim strokes mid-air
  if (m_isUnderwater) {
    float targetMaxSpeed =
        isRunningInput ? profile.underwaterRunMaxSpeed
                       : profile.underwaterWalkMaxSpeed;
    float newVx = currentVx;

    // Horizontal movement (sluggish underwater feel)
    if (inputDirX != 0.0f) {
      newVx += inputDirX * UNDERWATER_ACCEL * dt;
      newVx = std::clamp(newVx, -targetMaxSpeed, targetMaxSpeed);
    } else {
      float frictionStep = UNDERWATER_FRICTION * dt;
      if (newVx > 0.0f) {
        newVx = std::max(0.0f, newVx - frictionStep);
      } else if (newVx < 0.0f) {
        newVx = std::min(0.0f, newVx + frictionStep);
      }
      if (std::abs(newVx) < 1.0f)
        newVx = 0.0f;
    }

    // Swim stroke: jump key gives upward impulse
    if (jumpKeyPressed) {
      currentVy = UNDERWATER_SWIM_IMPULSE;
      EventBus::getInstance().notify(EventType::PLAYER_JUMPED);
    }

    // Cap sinking speed
    if (currentVy > UNDERWATER_MAX_SINK_SPEED) {
      currentVy = UNDERWATER_MAX_SINK_SPEED;
    }

    float newVxMeters = PhysicsEngine::pixelsToMeters(newVx);
    m_body->SetLinearVelocity(b2Vec2(newVxMeters, currentVy));
    return;
  }

  float targetMaxSpeed =
      isRunningInput ? profile.runMaxSpeed : profile.walkMaxSpeed;
  float newVx = currentVx;

  if (isGrounded()) {
    applyGroundPhysics(dt, inputDirX, isRunningInput, jumpKeyPressed, currentVy,
                       newVx, targetMaxSpeed);
  } else {
    applyAirPhysics(dt, inputDirX, jumpKeyReleased, currentVy, newVx,
                    targetMaxSpeed);
  }

  float newVxMeters = PhysicsEngine::pixelsToMeters(newVx);
  m_body->SetLinearVelocity(b2Vec2(newVxMeters, currentVy));
}

void Mario::initPhysics(b2World *world, b2BodyType type,
                        const sf::Vector2f &size, bool isSensor) {
  Entity::initPhysics(world, type, size, isSensor);
  rebuildFixture();
}

void Mario::rebuildFixture() {
  const sf::Vector2f targetSize = bodySizeForState(m_marioState);

  if (!m_body) {
    m_size = targetSize;
    return;
  }

  if (m_body->GetWorld() && m_body->GetWorld()->IsLocked()) {
    m_pendingFixtureRebuild = true;
    return;
  }
  m_pendingFixtureRebuild = false;

  // Preserve foot Y position on ground across SMALL <-> SUPER/FIRE fixture
  // transitions
  float oldHalfHeight = PhysicsEngine::pixelsToMeters(m_size.y / 2.0f);
  float newHalfHeight = PhysicsEngine::pixelsToMeters(targetSize.y / 2.0f);
  b2Vec2 pos = m_body->GetPosition();
  pos.y -= (newHalfHeight - oldHalfHeight);
  m_body->SetTransform(pos, m_body->GetAngle());

  m_size = targetSize;

  // Remove existing fixtures
  for (b2Fixture *f = m_body->GetFixtureList(); f;) {
    b2Fixture *next = f->GetNext();
    m_body->DestroyFixture(f);
    f = next;
  }

  // Create new shape with 0.0f friction to prevent wall sticking
  // Use a beveled octagon (chamfered box) to prevent Box2D ghost collisions on
  // seams between blocks
  b2PolygonShape dynamicBox;
  float w = PhysicsEngine::pixelsToMeters(targetSize.x / 2.0f);
  float h = PhysicsEngine::pixelsToMeters(targetSize.y / 2.0f);
  // A wider chamfer prevents a polygon-vs-box corner contact from generating
  // an upward impulse when Mario walks across a mixed B/?/B row backwards.
  const float bevel = PhysicsEngine::pixelsToMeters(8.0f);

  b2Vec2 vertices[8];
  vertices[0].Set(-w + bevel, -h); // Top edge, left
  vertices[1].Set(w - bevel, -h);  // Top edge, right
  vertices[2].Set(w, -h + bevel);  // Right edge, top
  vertices[3].Set(w, h - bevel);   // Right edge, bottom
  vertices[4].Set(w - bevel, h);   // Bottom edge, right
  vertices[5].Set(-w + bevel, h);  // Bottom edge, left
  vertices[6].Set(-w, h - bevel);  // Left edge, bottom
  vertices[7].Set(-w, -h + bevel); // Left edge, top

  dynamicBox.Set(vertices, 8);

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &dynamicBox;
  fixtureDef.density = MARIO_FIXTURE_DENSITY;
  fixtureDef.friction = MARIO_FIXTURE_FRICTION;
  m_body->CreateFixture(&fixtureDef);

  // Share the negative collision group used by FireBall so projectiles pass
  // THROUGH Mario physically instead of bumping/pushing him. Applied on every
  // fixture rebuild so the group survives Small<->Super/Fire state switches.
  if (b2Fixture *marioFixture = m_body->GetFixtureList()) {
    b2Filter filter = marioFixture->GetFilterData();
    filter.groupIndex = COLLISION_GROUP_PLAYER_PROJECTILE;
    marioFixture->SetFilterData(filter);
  }

#ifdef DEBUG
  std::cout << "[DEBUG][Mario] Rebuilt fixture for state size: ("
            << targetSize.x << ", " << targetSize.y << ")" << std::endl;
#endif
}

void Mario::jump() {
  if (!m_body || m_isDying)
    return;

  // Underwater: swim stroke works any time (not just grounded)
  if (m_isUnderwater) {
    m_jumpRequested = true;
    return;
  }

  if (!isGrounded())
    return;

  m_jumpRequested = true;
}

void Mario::releaseJump() { m_jumpReleased = true; }

void Mario::moveLeft() { setMoveIntent(-1.0f); }

void Mario::moveRight() { setMoveIntent(1.0f); }

void Mario::stopMoving() { setMoveIntent(0.0f); }

void Mario::setMoveIntent(float inputDirection) {
  m_inputDirX = std::clamp(inputDirection, -1.0f, 1.0f);
  if (m_isClimbing && m_inputDirX != 0.0f) {
    m_isClimbing = false;
    if (m_body)
      m_body->SetGravityScale(1.0f);
  }
  if (m_inputDirX < 0.0f) {
    setFacingDirection(Direction::LEFT);
  } else if (m_inputDirX > 0.0f) {
    setFacingDirection(Direction::RIGHT);
  }
}

void Mario::setVerticalIntent(float inputDirection) {
  m_verticalIntent = std::clamp(inputDirection, -1.0f, 1.0f);
}

void Mario::updateSpriteLayout() {
  if (!m_sprite) {
    return;
  }

  const sf::IntRect texRect = m_sprite->getTextureRect();
  if (texRect.size.x > 0 && texRect.size.y > 0) {
    const float spriteWidth = static_cast<float>(texRect.size.x) * 2.f;
    const float spriteHeight = static_cast<float>(texRect.size.y) * 2.f;
    const float groundY = m_position.y + m_size.y;
    const float spriteX = m_position.x + (m_size.x - spriteWidth) / 2.f;
    const float spriteY = groundY - spriteHeight;
    m_sprite->setPosition({spriteX, spriteY});
  }

  const float scaleX = (m_facingDirection == Direction::LEFT) ? -2.f : 2.f;
  m_sprite->setScale({scaleX, 2.f});
  m_sprite->setOrigin(m_facingDirection == Direction::LEFT
                          ? sf::Vector2f(16.f, 0.f)
                          : sf::Vector2f(0.f, 0.f));
}

void Mario::setClimbContext(bool onVine, float vineCenterX) {
  if (!onVine) {
    if (m_isClimbing && m_body)
      m_body->SetGravityScale(1.0f);
    m_isClimbing = false;
    return;
  }
  if (m_verticalIntent != 0.0f && m_inputDirX == 0.0f && !m_isUnderwater &&
      !m_isDying) {
    m_isClimbing = true;
    if (m_body) {
      m_body->SetGravityScale(0.0f);
      const float alignedLeft = vineCenterX - m_size.x / 2.0f;
      if (std::abs(m_position.x - alignedLeft) > 0.01f) {
        setPosition({alignedLeft, m_position.y});
      }
    }
  }
}

void Mario::setFlagpoleSliding(bool sliding) {
  m_isFlagpoleSliding = sliding;
  m_verticalIntent = 0.0f;
  m_jumpRequested = false;
  m_jumpReleased = false;
  if (sliding) {
    m_inputDirX = 0.0f;
  }
  if (m_body) {
    m_body->SetGravityScale(sliding ? 0.0f : 1.0f);
    if (!sliding) {
      m_body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
    }
  }
}

void Mario::beginFlagpoleSlide(float poleCenterX, float targetTopY) {
  m_flagpoleTargetTopY = targetTopY;
  setAutomaticWalkSpeed(0.0f);

  // Stand flush against the correct side of the pole: Mario grabs from the
  // left with the sprite facing right and from the right facing left. The
  // +/-14.0f offset snugs his body beside the pole column while
  // updateSpriteLayout() flips the sprite horizontally (never vertically).
  const bool marioIsRightOfPole = m_position.x + m_size.x / 2.0f > poleCenterX;
  setFacingDirection(marioIsRightOfPole ? Direction::LEFT : Direction::RIGHT);
  const float offsetX = marioIsRightOfPole ? 14.0f : -14.0f;
  m_flagpoleTargetX = poleCenterX - m_size.x / 2.0f + offsetX;

  setPosition({m_flagpoleTargetX, m_position.y});
  updateSpriteLayout();

  setFlagpoleSliding(true);
}

void Mario::updateFlagpoleSlide(float dt) {
  if (!m_isFlagpoleSliding || !m_body) {
    return;
  }

  // Lock Mario to the pole-adjacent column so physics drift cannot push him
  // off the pole during the scripted descent (zero horizontal drift).
  if (std::abs(m_position.x - m_flagpoleTargetX) > 0.01f) {
    setPosition({m_flagpoleTargetX, m_position.y});
  }

  if (m_position.y < m_flagpoleTargetTopY) {
    const float nextY = std::min(m_position.y + FLAGPOLE_SLIDE_SPEED * dt,
                                 m_flagpoleTargetTopY);
    setPosition({m_flagpoleTargetX, nextY});
  } else {
    setPosition({m_flagpoleTargetX, m_flagpoleTargetTopY});
  }

  // The descent is fully deterministic (independent of the fixed-step clamp),
  // so Box2D must not add its own motion on top of it.
  m_body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
}

bool Mario::isFlagpoleSlideComplete() const {
  // Allow for Box2D contact slop: when the pole base sits on the ground row,
  // Mario comes to rest with his feet on the floor tile a fraction of a pixel
  // above the exact target Y. A small tolerance lets the level treat the
  // descent as finished; the flag-drop gate still guards LEVEL_COMPLETED.
  constexpr float SLIDE_BASE_SLOP = 4.0f;
  return m_isFlagpoleSliding &&
         m_position.y >= m_flagpoleTargetTopY - SLIDE_BASE_SLOP;
}

void Mario::setAutomaticWalkSpeed(float speed) {
  m_automaticWalkSpeed = std::max(0.0f, speed);
}

bool Mario::hasCeilingClearance() const { return hasGrowthClearance(); }

bool Mario::applyStateTransition(MarioState state, bool withPresentation) {
  const bool currentSuperBody = m_size.y > SMALL_MARIO_SIZE.y + 0.01f;
  const bool targetSuperBody = usesSuperBody(state);
  if (m_marioState == state && currentSuperBody == targetSuperBody) {
    return false;
  }

  const bool isGrowth = !currentSuperBody && targetSuperBody;
  const bool isShrink = currentSuperBody && !targetSuperBody;
  const bool isFireUpgrade = !usesFire(m_marioState) && usesFire(state);
  const bool isFireDowngrade = usesFire(m_marioState) && !usesFire(state);
  const bool usesFireTransition = usesFire(m_marioState) || usesFire(state);
  const bool worldLocked =
      m_body && m_body->GetWorld() && m_body->GetWorld()->IsLocked();
  if (isGrowth && (worldLocked || !hasGrowthClearance())) {
    m_pendingGrowthState = state;
    m_pendingGrowthPresentation =
        m_pendingGrowthPresentation || withPresentation;
    return false;
  }

  m_pendingGrowthState = MarioState::SMALL;
  m_pendingGrowthPresentation = false;
  m_marioState = state;

  switch (state) {
  case MarioState::SMALL:
    m_statePattern = std::make_unique<SmallMarioState>();
    break;
  case MarioState::SUPER:
    m_statePattern = std::make_unique<SuperMarioState>();
    break;
  case MarioState::FIRE_SMALL:
    m_statePattern = std::make_unique<SmallFireMarioState>();
    break;
  case MarioState::FIRE_SUPER:
    m_statePattern = std::make_unique<SuperFireMarioState>();
    break;
  }

  setupAnimationsForState(*m_animationSystem, m_marioState, m_characterType);
  playAnimation("idle");
  rebuildFixture(); // Single foot-anchor authority.
  if (m_body && !worldLocked) {
    syncPhysics();
  }

  if (withPresentation) {
    m_isTransforming = true;
    m_transformTimer = TRANSFORM_PRESENTATION_DURATION;
    if (isFireUpgrade || isFireDowngrade ||
        (usesFireTransition && !isGrowth && !isShrink)) {
      const bool isSuper = usesSuperBody(state) || usesSuperBody(m_marioState);
      const auto &sequence =
          isSuper
              ? SpriteFrames::shared::GrowShrink::FireMario::bigFireSequence()
              : SpriteFrames::shared::GrowShrink::FireMario::smallFireSequence();
      m_animationSystem->addAnimation(
          "transform",
          AnimationSystem::createManualAnimation(
              sequence, TRANSFORM_FRAME_DURATION, false));
      playAnimation("transform");
    } else if (isGrowth) {
      const auto &sequence =
          m_characterType == CharacterType::LUIGI
              ? SpriteFrames::shared::GrowShrink::Luigi::growSequence()
              : (usesFireTransition
                     ? SpriteFrames::shared::GrowShrink::FireMario::growSequence()
                     : SpriteFrames::shared::GrowShrink::Mario::growSequence());
      m_animationSystem->addAnimation(
          "transform",
          AnimationSystem::createManualAnimation(
              sequence, TRANSFORM_FRAME_DURATION, false));
      playAnimation("transform");
    } else if (isShrink) {
      const auto &sequence =
          m_characterType == CharacterType::LUIGI
              ? SpriteFrames::shared::GrowShrink::Luigi::shrinkSequence()
              : (usesFireTransition
                     ? SpriteFrames::shared::GrowShrink::FireMario::shrinkSequence()
                     : SpriteFrames::shared::GrowShrink::Mario::shrinkSequence());
      m_animationSystem->addAnimation(
          "transform",
          AnimationSystem::createManualAnimation(
              sequence, TRANSFORM_FRAME_DURATION, false));
      playAnimation("transform");
    } else {
      const auto &sequence =
          m_characterType == CharacterType::LUIGI
              ? SpriteFrames::shared::GrowShrink::Luigi::growSequence()
              : (usesFireTransition
                     ? SpriteFrames::shared::GrowShrink::FireMario::growSequence()
                     : SpriteFrames::shared::GrowShrink::Mario::growSequence());
      m_animationSystem->addAnimation(
          "transform",
          AnimationSystem::createManualAnimation(
              sequence, TRANSFORM_FRAME_DURATION, false));
      playAnimation("transform");
    }
  }

  if (m_sprite) {
    // Fire frames contain their own red/white palette. Keep the sprite
    // un-tinted so skin, clothes, and cap colors remain distinct.
    m_sprite->setColor(sf::Color::White);
  }
  return true;
}

void Mario::powerUp(MarioState state) {
  // A power-up is monotonic; it must never act as a power-down or remove FIRE.
  const bool validUpgrade =
      (m_marioState == MarioState::SMALL &&
       (state == MarioState::SUPER || state == MarioState::FIRE_SMALL)) ||
      (m_marioState == MarioState::SUPER && state == MarioState::FIRE_SUPER) ||
      (m_marioState == MarioState::FIRE_SMALL &&
       state == MarioState::FIRE_SUPER);
  if (!validUpgrade) {
    return;
  }

  applyStateTransition(state, true);
  // Pickup score/SFX/event happen once even if geometry defers the fixture.
  EventBus::getInstance().notify(EventType::PLAYER_POWER_UP);
}

void Mario::setCharacterType(CharacterType type) {
  m_characterType = type;
  m_jumpForce = characterProfileFor(m_characterType).jumpForce;
  setupAnimationsForState(*m_animationSystem, m_marioState, m_characterType);
  // Refresh presentation without mutating any gameplay/lifecycle state. Keep
  // a compatible active clip where possible so changing identity during a
  // movement, climb, or death phase does not reset that phase to idle.
  const std::string currentAnimation =
      m_animationSystem->getCurrentAnimationName();
  if (m_isDying || currentAnimation == "death") {
    playAnimation("death");
  } else if (m_isFlagpoleSliding || currentAnimation == "climb") {
    playAnimation("climb");
  } else if (m_isClimbing && currentAnimation == "climb_up") {
    playAnimation("climb_up");
  } else if (m_isClimbing && currentAnimation == "climb_down") {
    playAnimation("climb_down");
  } else if (m_isClimbing && currentAnimation == "climb_idle") {
    playAnimation("climb_idle");
  } else if (currentAnimation == "walk" || currentAnimation == "jump" ||
             currentAnimation == "skid" || currentAnimation == "swim" ||
             currentAnimation == "action" || currentAnimation == "spawn") {
    playAnimation(currentAnimation);
  } else {
    playAnimation("idle");
  }
  updateAnimation(0.0f);
  updateSpriteLayout();
}

void Mario::powerDown() {
  if (isDamageImmune() || isStarInvincible() || m_isDying || m_isTransforming) {
    return;
  }
  if (m_body && m_body->GetWorld() && m_body->GetWorld()->IsLocked()) {
    queuePowerDown();
    return;
  }

  if (m_marioState == MarioState::FIRE_SUPER) {
    applyStateTransition(MarioState::SUPER, true);
    activateDamageGrace(DAMAGE_INVINCIBILITY_DURATION);
    EventBus::getInstance().notify(EventType::PLAYER_POWER_DOWN);
  } else if (m_marioState == MarioState::FIRE_SMALL) {
    applyStateTransition(MarioState::SMALL, true);
    activateDamageGrace(DAMAGE_INVINCIBILITY_DURATION);
    EventBus::getInstance().notify(EventType::PLAYER_POWER_DOWN);
  } else if (m_marioState == MarioState::SUPER) {
    applyStateTransition(MarioState::SMALL, true);
    activateDamageGrace(DAMAGE_INVINCIBILITY_DURATION);
    EventBus::getInstance().notify(EventType::PLAYER_POWER_DOWN);
  } else {
    loseLife();
  }
}

void Mario::queuePowerDown() {
  if (!isInvincible()) {
    m_pendingPowerDown = true;
  }
}

void Mario::loseLife() {
  if (m_isDying)
    return;

  m_isDying = true;
  m_deathAnimationFinished = false;
  m_deathFallDelayTimer = DEATH_POST_ANIMATION_FALL_DELAY;
  m_isSpawning = false;
  m_isTransforming = false;
  m_transformTimer = 0.f;
  m_inputDirX = 0.f;
  m_jumpRequested = false;
  m_jumpReleased = false;
  m_isRunning = false;
  resetStompScoreChain();
  m_pendingStompScoreAwards.clear();
  m_isClimbing = false;
  m_isFlagpoleSliding = false;
  m_verticalIntent = 0.0f;
  m_automaticWalkSpeed = 0.0f;
  m_isInvincible = false;
  m_invincibilityTimer = 0.0f;
  m_isStarInvincible = false;
  m_starInvincibilityTimer = 0.0f;

  if (m_lives > 0) {
    m_lives--;
  }

  // Reset to SMALL state and reload animations for character type immediately.
  // applyStateTransition also keeps the death sprite anchored to Mario's feet
  // when a Super/Fire body shrinks back to the death form.
  applyStateTransition(MarioState::SMALL, false);
  playAnimation("death");
  // loseLife can be called after a collision callback, while Level will not
  // update newly selected animations until later in the frame. Set the death
  // frame now so the old walk/idle frame never flashes on screen.
  updateAnimation(0.f);
  updateSpriteLayout();

  if (m_sprite) {
    m_sprite->setColor(sf::Color::White);
  }

  if (m_body) {
    setVelocity({0.f, -DEATH_JUMP_SPEED});
    m_body->SetGravityScale(1.0f);
    for (b2Fixture *f = m_body->GetFixtureList(); f; f = f->GetNext()) {
      f->SetSensor(true);
    }
  }

#ifdef DEBUG
  std::cout << "[DEBUG][Mario] Mario died. Lives remaining: " << m_lives
            << std::endl;
#endif

  EventBus::getInstance().notify(EventType::PLAYER_DIED);
}

bool Mario::isDeathAnimationFinished() const {
  return m_deathAnimationFinished;
}

void Mario::respawn(const sf::Vector2f &spawnPosition) {
  m_marioState = MarioState::SMALL;
  m_pendingGrowthState = MarioState::SMALL;
  m_pendingGrowthPresentation = false;
  m_pendingFixtureRebuild = false;
  m_pendingPowerDown = false;
  m_size = SMALL_MARIO_SIZE;
  m_health = DEFAULT_MARIO_HEALTH;
  m_active = true;
  m_isDying = false;
  m_deathAnimationFinished = false;
  m_deathFallDelayTimer = 0.0f;
  m_isSpawning = true;
  m_isInvincible = false;
  m_isStarInvincible = false;
  m_invincibilityTimer = 0.f;
  m_starInvincibilityTimer = 0.f;
  m_fireCooldown = 0.f;
  m_isTransforming = false;
  m_transformTimer = 0.f;
  m_inputDirX = 0.f;
  m_verticalIntent = 0.f;
  m_jumpRequested = false;
  m_jumpReleased = false;
  m_isRunning = false;
  m_isSkidding = false;
  m_isClimbing = false;
  m_isFlagpoleSliding = false;
  m_flagpoleTargetTopY = 0.f;
  m_automaticWalkSpeed = 0.f;
  m_markedForRemoval = false;
  m_pendingDestroy = false;
  m_velocity = {0.f, 0.f};
  resetStompScoreChain();
  m_pendingStompScoreAwards.clear();
  clearGroundedState();
  m_statePattern = std::make_unique<SmallMarioState>();

  if (m_body) {
    m_body->SetEnabled(true);
    m_body->SetGravityScale(1.0f);
    m_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
    m_body->SetAngularVelocity(0.f);
    m_body->SetAwake(true);
  }
  setupAnimationsForState(*m_animationSystem, m_marioState, m_characterType);
  playAnimation("spawn");
  rebuildFixture();
  setPosition(spawnPosition);

  if (m_sprite) {
    m_sprite->setColor(sf::Color::White);
  }
}

void Mario::setRespawnPosition(const sf::Vector2f &spawnPosition) {
  m_respawnPosition = spawnPosition;
}

MarioState Mario::getMarioState() const { return m_marioState; }

bool Mario::hasGrowthClearance() const {
  if (!m_body || !m_body->GetWorld())
    return true;

  float currentHalfHeight = PhysicsEngine::pixelsToMeters(m_size.y / 2.0f);
  float superHalfHeight =
      PhysicsEngine::pixelsToMeters(SUPER_MARIO_SIZE.y / 2.0f);
  float heightDiff = (superHalfHeight - currentHalfHeight) * 2.0f;
  if (heightDiff <= 0.001f)
    return true;

  b2AABB queryAABB;
  b2Vec2 pos = m_body->GetPosition();
  float halfWidth = PhysicsEngine::pixelsToMeters(m_size.x / 2.0f) - 0.05f;
  queryAABB.lowerBound.Set(pos.x - halfWidth,
                           pos.y - currentHalfHeight - heightDiff);
  queryAABB.upperBound.Set(pos.x + halfWidth, pos.y - currentHalfHeight);

  class ClearanceQueryCallback : public b2QueryCallback {
  public:
    b2Body *marioBody;
    bool blocked = false;
    ClearanceQueryCallback(b2Body *body) : marioBody(body) {}
    bool ReportFixture(b2Fixture *fixture) override {
      if (!fixture || fixture->IsSensor())
        return true;
      if (fixture->GetBody() == marioBody)
        return true;
      blocked = true;
      return false;
    }
  } callback(m_body);

  m_body->GetWorld()->QueryAABB(&callback, queryAABB);
  return !callback.blocked;
}

void Mario::setMarioState(MarioState state) {
  applyStateTransition(state, false);
}

void Mario::addScore(int points) { m_score += points; }

StompScoreAward Mario::awardStompScore(const sf::Vector2f &position) {
  std::size_t awardIndex = m_stompScoreChainIndex;

  // SMB1 treats enemies stomped in the same bounce/contact batch specially:
  // the second enemy skips one value (a fresh double stomp is 100 + 400).
  if (m_stompsInCurrentPhysicsStep > 0) {
    ++awardIndex;
  }

  StompScoreAward award;
  award.position = position;
  if (awardIndex >= STOMP_SCORE_CHAIN.size()) {
    award.grantsLife = true;
    addLife();
    EventBus::getInstance().notify(EventType::ONE_UP_COLLECTED);
  } else {
    award.points = STOMP_SCORE_CHAIN[awardIndex];
    addScore(award.points);
  }

  m_stompScoreChainIndex = awardIndex + 1;
  ++m_stompsInCurrentPhysicsStep;
  m_pendingStompScoreAwards.push_back(award);
  return award;
}

std::vector<StompScoreAward> Mario::consumePendingStompScoreAwards() {
  m_stompsInCurrentPhysicsStep = 0;
  std::vector<StompScoreAward> pending =
      std::move(m_pendingStompScoreAwards);
  m_pendingStompScoreAwards.clear();
  return pending;
}

void Mario::resetStompScoreChain() {
  m_stompScoreChainIndex = 0;
  m_stompsInCurrentPhysicsStep = 0;
}

void Mario::addCoin() { ++m_coinCount; }

void Mario::collectCoin(int scoreValue) {
  addCoin();
  if (scoreValue == ScoreRules::pointsFor(ScoreEvent::COIN_COLLECTED)) {
    ScoreRules::award(*this, ScoreEvent::COIN_COLLECTED);
  } else {
    // Keep the legacy overload useful for callers with a custom score while
    // the normal collectible path uses the central score catalog above.
    addScore(scoreValue);
  }
  EventBus::getInstance().notify(EventType::COIN_COLLECTED);
}

int Mario::getScore() const { return m_score; }

int Mario::getCoinCount() const { return m_coinCount; }

void Mario::setScore(int score) { m_score = score; }

void Mario::setCoinCount(int coins) { m_coinCount = coins; }

bool Mario::tryStartFireBallShot() {
  if (!canShootFireBall() || m_isDying || !m_active || m_isTransforming ||
      m_isFlagpoleSliding) {
    return false;
  }

  m_fireCooldown = FIRE_COOLDOWN_DURATION;
  playAnimation("action");
  return true;
}

void Mario::activateStarman(float duration) { setStarInvincible(duration); }

void Mario::activateDamageGrace(float duration) {
  m_invincibilityTimer = duration;
  m_isInvincible = true;
}

void Mario::setInvincible(float duration) { activateDamageGrace(duration); }

bool Mario::isDamageImmune() const { return m_invincibilityTimer > 0.0f; }

void Mario::setStarInvincible(float duration) {
  m_isStarInvincible = true;
  m_starInvincibilityTimer = duration;
}

bool Mario::isStarInvincible() const { return m_isStarInvincible; }

void Mario::updateInvincibility(float dt) {
  bool starExpired = false;
  if (m_isStarInvincible) {
    m_starInvincibilityTimer = std::max(0.0f, m_starInvincibilityTimer - dt);
    if (m_starInvincibilityTimer <= 0.f) {
      m_isStarInvincible = false;
      starExpired = true;
    }
  }

  if (m_invincibilityTimer > 0.0f) {
    m_invincibilityTimer = std::max(0.0f, m_invincibilityTimer - dt);
  }
  m_isInvincible = m_invincibilityTimer > 0.0f;

  if (m_sprite && !m_isTransforming) {
    // Do not recolor Fire Mario: the selected Fire atlas already supplies the
    // correct palette. This color is only the neutral base for blink effects.
    const sf::Color baseColor = sf::Color::White;
    if (m_isStarInvincible) {
      static const sf::Color rainbow[] = {
          sf::Color(255, 60, 60),   sf::Color(255, 200, 40),
          sf::Color(60, 255, 60),   sf::Color(60, 220, 255),
          sf::Color(255, 100, 255), sf::Color::White};
      int colorIndex =
          (static_cast<int>(m_starInvincibilityTimer * 15.f) % 6 + 6) % 6;
      m_sprite->setColor(rainbow[colorIndex]);
    } else if (m_isInvincible) {
      const int milliseconds = static_cast<int>(m_invincibilityTimer * 1000.f);
      m_sprite->setColor((milliseconds / 100) % 2 == 0 ? sf::Color::Transparent
                                                       : baseColor);
    } else {
      m_sprite->setColor(baseColor);
    }
  }

  if (starExpired) {
    EventBus::getInstance().notify(EventType::PLAYER_INVINCIBILITY_EXPIRED);
  }
}

bool Mario::isInvincible() const {
  return m_isInvincible || m_isStarInvincible;
}

bool Mario::canShootFireBall() const {
  return usesFire(m_marioState) && m_fireCooldown <= 0.0f;
}

bool Mario::canBreakBricks() const {
  return (m_statePattern && m_statePattern->canBreakBricks()) ||
         isStarInvincible();
}

int Mario::getLives() const { return m_lives; }

void Mario::setLives(int lives) { m_lives = lives; }

bool Mario::isRunning() const { return m_isRunning; }

void Mario::setRunIntent(bool running) { m_isRunning = running; }

bool Mario::isSkidding() const { return m_isSkidding; }

bool Mario::isDying() const { return m_isDying; }

void Mario::refreshGroundedState() {
  if (!m_body) {
    setGrounded(false);
    return;
  }

  bool grounded = false;
  for (b2ContactEdge *edge = m_body->GetContactList(); edge;
       edge = edge->next) {
    b2Contact *contact = edge->contact;
    if (!contact || !contact->IsTouching()) {
      continue;
    }

    b2Body *bodyA = contact->GetFixtureA()->GetBody();
    b2Body *bodyB = contact->GetFixtureB()->GetBody();
    b2Body *otherBody = (bodyA == m_body) ? bodyB : bodyA;
    if (!otherBody) {
      continue;
    }

    const uintptr_t userData = otherBody->GetUserData().pointer;
    Entity *otherEntity = (userData != 0 && !TileMap::isTileUserData(userData))
                              ? reinterpret_cast<Entity *>(userData)
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
  if (grounded) {
    resetStompScoreChain();
  }
}

void Mario::clearGroundedState() { setGrounded(false); }

void Mario::onCollisionBegin(Entity *other, b2Contact *contact,
                             const b2Vec2 &normal) {
  // CollisionManager is the sole gameplay collision authority. The legacy
  // double-dispatch hook remains part of Entity's interface for compatibility,
  // but this callback intentionally has no gameplay side effects.
  (void)other;
  (void)contact;
  (void)normal;
}

void Mario::onCollisionEnd(Entity *other, b2Contact *contact) {
  (void)other;
  (void)contact;
}
