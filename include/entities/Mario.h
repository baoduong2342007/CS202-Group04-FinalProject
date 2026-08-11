/**
 * @file Mario.h
 * @author TV3 (Bảo)
 * @brief Mario player character class with authentic NES movement physics and states
 * @note Sprint 4: lives, skidding, FireBall support; score & invincibility from develop
 */

#pragma once

#include <memory>
#include "entities/Character.h"
#include "states/IMarioState.h"

class FireBall;

// Define Mario's power-up states strictly according to project specifications
enum class MarioState {
    SMALL,
    SUPER,
    FIRE
};

// Selectable player character identity (Mario / Luigi)
enum class CharacterType {
    MARIO,
    LUIGI
};

class Mario : public Character {
public:
    // 1. Constructor / Destructor
    Mario();
    Mario(const sf::Vector2f& position, const sf::Vector2f& size);
    ~Mario() override = default;

    // 2. Override methods
    void initPhysics(b2World* world, b2BodyType type, const sf::Vector2f& size, bool isSensor = false) override;
    void update(float dt) override;

    // 3. Public methods
    /// @deprecated Legacy input handler — replaced by Command pattern (InputHandler).
    ///             Only used in Box2DDemo.cpp. Prefer jump()/moveLeft()/moveRight()/stopMoving().
    [[deprecated("Use Command pattern via InputHandler instead")]]
    void handleInput();
    void jump();
    void releaseJump();
    void moveLeft();
    void moveRight();
    void stopMoving();
    void setMoveIntent(float inputDirection);
    void preparePhysics(float dt);
    void powerUp(MarioState state);
    void powerDown();
    void queuePowerDown();
    void addScore(int points);
    void addCoin();
    void collectCoin(int scoreValue = 100);
    std::unique_ptr<FireBall> shootFireBall(b2World* world);
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
    bool isMario() const override { return true; }

    // Double Dispatch collision overrides
    void onCollisionBegin(Entity* other, b2Contact* contact, const b2Vec2& normal) override;
    void onCollisionEnd(Entity* other, b2Contact* contact) override;

    // 4. Getters / Setters
    MarioState getMarioState() const;
    void setMarioState(MarioState state);
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
    void setInvincible(float duration);
    bool canShootFireBall() const;
    int getLives() const;
    void setLives(int lives);

    CharacterType getCharacterType() const { return m_characterType; }
    void setCharacterType(CharacterType type);

    bool isRunning() const;
    /// Set the per-frame run intent consumed by preparePhysics().
    void setRunIntent(bool running);
    void setRunning(bool running) { setRunIntent(running); }
    bool isSkidding() const;
    bool isDying() const;
    bool isDeathAnimationFinished() const;
    bool isTransforming() const { return m_isTransforming; }

protected:
    // 5. Protected methods
    void rebuildFixture();
    void applyMovementPhysics(float dt, float inputDirX, bool isRunningInput, bool jumpKeyPressed, bool jumpKeyReleased);
    void applyGroundPhysics(float dt, float inputDirX, bool isRunningInput, bool jumpKeyPressed, float& currentVy, float& newVx, float targetMaxSpeed);
    void applyAirPhysics(float dt, float inputDirX, bool jumpKeyReleased, float& currentVy, float& newVx, float targetMaxSpeed);

    // 6. Protected / Private members
    MarioState m_marioState;
    CharacterType m_characterType = CharacterType::MARIO;
    std::unique_ptr<class IMarioState> m_statePattern;
    float m_jumpForce;
    float m_moveSpeed;
    int m_score;
    int m_coinCount;
    bool m_isInvincible;
    float m_invincibilityTimer;
    bool m_isStarInvincible = false;
    float m_starInvincibilityTimer = 0.f;
    int m_lives;

    bool m_isDying;
    bool m_isSpawning;
    bool m_isRunning;
    bool m_isSkidding;
    bool m_wasJumpPressed;

    // Transformation effect
    bool m_isTransforming;
    float m_transformTimer;

    // Fixture Caching
    b2Fixture* m_smallFixture = nullptr;
    b2Fixture* m_superFixture = nullptr;

    bool m_pendingFixtureRebuild = false;
    bool m_pendingPowerDown = false;
    MarioState m_pendingGrowthState = MarioState::SMALL;
    sf::Vector2f m_respawnPosition;

    float m_inputDirX = 0.0f;
    bool m_jumpRequested = false;
    bool m_jumpReleased = false;

    float m_fireCooldown = 0.0f;
    static constexpr float FIRE_COOLDOWN_DURATION = 0.25f; // 250ms per S6-TV3-18

    // Dynamic Pit Threshold & Map Bounds (S6-TV3-07)
    float m_pitThreshold = 800.0f;

public:
    void setPitThreshold(float threshold) { m_pitThreshold = threshold; }
    float getPitThreshold() const { return m_pitThreshold; }
    bool hasGrowthClearance() const;
};
