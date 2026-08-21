/**
 * @file Mario.h
 * @author TV3 (Bảo)
 * @brief Mario player character class with authentic NES movement physics and states
 * @note Sprint 6 TV3: foot-anchored fixture rebuilding, low-ceiling clearance queries, independent invincibility clocks, deterministic death/respawn, and fixture-derived bounds
 */

#pragma once

#include <memory>
#include <vector>
#include "entities/Character.h"
#include "states/IMarioState.h"

// Mario's power-up states encode both body tier and Fire capability.
enum class MarioState {
    SMALL = 0,
    SUPER = 1,
    FIRE_SMALL = 2,
    FIRE_SUPER = 3
};

// Selectable player character identity (Mario / Luigi)
enum class CharacterType {
    MARIO,
    LUIGI
};

/// Intentional gameplay differences for each selectable character.
/// All values use the units consumed by Mario's movement implementation.
struct CharacterProfile {
    float jumpForce;
    float walkMaxSpeed;
    float runMaxSpeed;
    float underwaterWalkMaxSpeed;
    float underwaterRunMaxSpeed;
};

/// Deferred world-space feedback produced by an accepted stomp transaction.
struct StompScoreAward {
    int points = 0;
    bool grantsLife = false;
    sf::Vector2f position{0.f, 0.f};
};

/// Return the immutable movement profile for a character identity.
constexpr CharacterProfile characterProfileFor(CharacterType type) noexcept {
    if (type == CharacterType::LUIGI) {
        return {510.0f, 160.0f, 250.0f, 90.0f, 144.0f};
    }
    return {460.0f, 175.0f, 280.0f, 100.0f, 160.0f};
}

class Mario : public Character {
public:
    // 1. Constructor / Destructor
    Mario();
    Mario(const sf::Vector2f& position, const sf::Vector2f& size);
    ~Mario() override = default;

    // 2. Override methods
    void initPhysics(b2World* world, b2BodyType type, const sf::Vector2f& size, bool isSensor = false) override;
    void update(float dt) override;
    void syncPhysics() override;

    // 3. Public methods
    void jump();
    void releaseJump();
    void moveLeft();
    void moveRight();
    void stopMoving();
    void setMoveIntent(float inputDirection);
    void setVerticalIntent(float inputDirection);
    void setClimbContext(bool onVine, float vineCenterX = 0.0f);
    bool isClimbing() const { return m_isClimbing; }
    void setFlagpoleSliding(bool sliding);
    void beginFlagpoleSlide(float poleCenterX, float targetTopY);
    void updateFlagpoleSlide(float dt);
    /// Limit Mario's automatic walk speed during the flagpole exit sequence.
    /// Pass a non-positive value to restore the normal movement cap.
    void setAutomaticWalkSpeed(float speed);
    bool isFlagpoleSliding() const { return m_isFlagpoleSliding; }
    /// True once Mario has descended the full flagpole to the bottom (targetTopY).
    bool isFlagpoleSlideComplete() const;
    static constexpr float FLAGPOLE_SLIDE_SPEED = 120.0f;
    void preparePhysics(float dt);
    /// Apply a monotonic gameplay power-up.
    void powerUp(MarioState state);
    void powerDown();
    void queuePowerDown();
    void addScore(int points);
    /// Queue an arbitrary score popup feedback (and apply points/life).
    void queueScoreAward(const sf::Vector2f& position, int points, bool grantsLife = false);
    /// Award the next airborne stomp-chain value and queue its popup.
    StompScoreAward awardStompScore(const sf::Vector2f& position);
    /// Drain popups after the Box2D step and begin a new simultaneity window.
    std::vector<StompScoreAward> consumePendingStompScoreAwards();
    /// End the current airborne stomp chain (normally when touching ground).
    void resetStompScoreChain();
    void addCoin();
    void collectCoin(int scoreValue = 100);
    /// Reserve an accepted shot by consuming Mario's authoritative cooldown.
    /// Level owns projectile construction and the active-projectile limit.
    bool tryStartFireBallShot();
    void setInvincible(float duration);
    void updateInvincibility(float dt);
    void loseLife();
    void addLife(int lives = 1) { m_lives += lives; }
    void respawn(const sf::Vector2f& spawnPosition);
    void setRespawnPosition(const sf::Vector2f& spawnPosition);
    /// Recompute grounding from the contacts that survived the latest Box2D step.
    void refreshGroundedState();
    /// Clear grounding when Mario starts an upward movement before the next step.
    void clearGroundedState();
    /// Checks if 32px overhead space is clear of solid terrain before growing (S6-TV3-11).
    bool hasCeilingClearance() const;
    EntityType getType() const override { return EntityType::MARIO; }
    EntitySubtype getSubtype() const noexcept override { return EntitySubtype::MARIO; }

    // Double Dispatch collision overrides
    void onCollisionBegin(Entity* other, b2Contact* contact, const b2Vec2& normal) override;
    void onCollisionEnd(Entity* other, b2Contact* contact) override;

    // 4. Getters / Setters
    MarioState getMarioState() const;
    void setMarioState(MarioState state);
    /// True when Mario can shatter brick blocks, including active Star power.
    bool canBreakBricks() const;
    int getScore() const;
    int getCoinCount() const;
    /// Sprint 6 (S6-TV1-10): setter for restoring session progress on level load.
    void setScore(int score);
    /// Sprint 6 (S6-TV1-10): setter for restoring session progress on level load.
    void setCoinCount(int coins);
    bool isInvincible() const;
    bool isStarInvincible() const;
    bool isDamageImmune() const;
    void setStarInvincible(float duration);
    void activateStarman(float duration = 10.0f);
    void activateDamageGrace(float duration = 2.0f);
    bool canShootFireBall() const;
    int getLives() const;
    void setLives(int lives);
    float getVerticalIntent() const { return m_verticalIntent; }
    float getHorizontalIntent() const { return m_inputDirX;}

    CharacterType getCharacterType() const { return m_characterType; }
    void setCharacterType(CharacterType type);
    CharacterProfile getCharacterProfile() const noexcept {
        return characterProfileFor(m_characterType);
    }
    float getJumpForce() const noexcept { return m_jumpForce; }

    /// Fixture collision group applied on every fixture (re)build.
    /// The default makes this Mario share the campaign projectile group so
    /// fireballs pass through him. PvP assigns a distinct negative group per
    /// player so each fighter's fireballs hit the opponent but not himself.
    void setFixtureCollisionGroup(int16_t group);
    int16_t getFixtureCollisionGroup() const { return m_fixtureCollisionGroup; }

    bool isRunning() const;
    /// Set the per-frame run intent consumed by preparePhysics().
    void setRunIntent(bool running);
    void setRunning(bool running) { setRunIntent(running); }
    bool isSkidding() const;
    bool isDying() const;
    /// True while Mario must not participate in gameplay collisions.
    bool isCollisionLocked() const { return !m_active || m_isDying || m_pendingPowerDown; }
    bool isDeathAnimationFinished() const;
    bool isTransforming() const { return m_isTransforming; }
    bool hasPendingGrowth() const { return m_pendingGrowthState != MarioState::SMALL; }
    MarioState getPendingGrowthState() const { return m_pendingGrowthState; }
    float getFireCooldownRemaining() const { return m_fireCooldown; }

    /// Underwater swim mode — Level sets this for UNDERWATER theme
    void setUnderwater(bool underwater) { m_isUnderwater = underwater; }
    bool isUnderwater() const { return m_isUnderwater; }

    /// Top ceiling boundary clamp mode (e.g. for enclosed arenas / PvP)
    void setCeilingClampEnabled(bool enabled) { m_ceilingClampEnabled = enabled; }
    bool isCeilingClampEnabled() const { return m_ceilingClampEnabled; }

    void updateSpriteLayout();
    void updateVisuals(float dt) {
        updateAnimation(dt);
        updateSpriteLayout();
    }

protected:
    // 5. Protected methods
    bool applyStateTransition(MarioState state, bool withPresentation);
    void rebuildFixture();
    bool handleDeathPhase(float dt);
    bool handleTransformPhase(float dt);
    bool handleSpawnPhase(float dt);
    void updateMovementAnimations(float dt);
    void applyWorldBoundsClamp();
    void applyMovementPhysics(float dt, float inputDirX, bool isRunningInput, bool jumpKeyPressed, bool jumpKeyReleased);
    void applyGroundPhysics(float dt, float inputDirX, bool isRunningInput, bool jumpKeyPressed, float& currentVy, float& newVx, float targetMaxSpeed);
    void applyAirPhysics(float dt, float inputDirX, bool jumpKeyReleased, float& currentVy, float& newVx, float targetMaxSpeed);

    // 6. Protected / Private members
    MarioState m_marioState;
    CharacterType m_characterType = CharacterType::MARIO;
    std::unique_ptr<class IMarioState> m_statePattern;
    float m_jumpForce;
    int16_t m_fixtureCollisionGroup = COLLISION_GROUP_PLAYER_PROJECTILE;
    int m_score;
    int m_coinCount;
    std::size_t m_stompScoreChainIndex = 0;
    std::size_t m_stompsInCurrentPhysicsStep = 0;
    std::vector<StompScoreAward> m_pendingStompScoreAwards;
    bool m_isInvincible;
    float m_invincibilityTimer;
    bool m_isStarInvincible = false;
    float m_starInvincibilityTimer = 0.f;
    int m_lives;

    bool m_isDying;
    bool m_isSpawning;
    bool m_isRunning;
    bool m_isSkidding;

    // Transformation effect
    bool m_isTransforming;
    float m_transformTimer;

    bool m_pendingFixtureRebuild = false;
    bool m_pendingPowerDown = false;
    MarioState m_pendingGrowthState = MarioState::SMALL;
    bool m_pendingGrowthPresentation = false;
    bool m_deathAnimationFinished = false;
    float m_deathFallDelayTimer = 0.0f;
    sf::Vector2f m_respawnPosition;

    float m_inputDirX = 0.0f;
    bool m_jumpRequested = false;
    bool m_jumpReleased = false;

    float m_fireCooldown = 0.0f;
    static constexpr float FIRE_COOLDOWN_DURATION = 0.25f; // 250ms per S6-TV3-18

    // Dynamic Pit Threshold & Map Bounds (S6-TV3-07)
    float m_pitThreshold = 800.0f;

    // Underwater swimming mode
    bool m_isUnderwater = false;
    bool m_ceilingClampEnabled = false;
    bool m_isClimbing = false;
    bool m_isFlagpoleSliding = false;
    float m_flagpoleTargetTopY = 0.0f;
    /// Pole-adjacent column Mario is locked to for the whole scripted descent.
    float m_flagpoleTargetX = 0.0f;
    float m_automaticWalkSpeed = 0.0f;
    float m_verticalIntent = 0.0f;

public:
    void setPitThreshold(float threshold) { m_pitThreshold = threshold; }
    float getPitThreshold() const { return m_pitThreshold; }
    bool hasGrowthClearance() const;
};
