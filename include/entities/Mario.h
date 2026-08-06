/**
 * @file Mario.h
 * @author TV3 (Bảo)
 * @brief Mario player character class with authentic NES movement physics and states
 * @note Sprint 4: lives, skidding, FireBall support; score & invincibility from develop
 */

#pragma once

#include "entities/Character.h"

// Define Mario's power-up states strictly according to project specifications
enum class MarioState {
    SMALL,
    SUPER,
    FIRE
};

class Mario : public Character {
public:
    // 1. Constructor / Destructor
    Mario();
    Mario(const sf::Vector2f& position, const sf::Vector2f& size);
    ~Mario() override = default;

    // 2. Override methods
    void update(float dt) override;

    // 3. Public methods
    /// @deprecated Legacy input handler — replaced by Command pattern (InputHandler).
    ///             Only used in Box2DDemo.cpp. Prefer jump()/moveLeft()/moveRight()/stopMoving().
    [[deprecated("Use Command pattern via InputHandler instead")]]
    void handleInput();
    void jump();
    void moveLeft();
    void moveRight();
    void stopMoving();
    void setMoveIntent(float inputDirection);
    void preparePhysics(float dt);
    void powerUp(MarioState state);
    void powerDown();
    void queuePowerDown();
    void addScore(int points);
    void collectCoin(int scoreValue = 200);
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
    bool isMario() const override { return true; }

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
    bool canShootFireBall() const;
    int getLives() const;
    void setLives(int lives);

    bool isRunning() const;
    bool isSkidding() const;

protected:
    // 5. Protected methods
    void rebuildFixture();
    void applyMovementPhysics(float dt, float inputDirX, bool isRunningInput, bool jumpKeyPressed, bool jumpKeyReleased);
    void applyGroundPhysics(float dt, float inputDirX, bool isRunningInput, bool jumpKeyPressed, float& currentVy, float& newVx, float targetMaxSpeed);
    void applyAirPhysics(float dt, float inputDirX, bool jumpKeyReleased, float& currentVy, float& newVx, float targetMaxSpeed);

    // 6. Protected / Private members
    MarioState m_marioState;
    float m_jumpForce;
    float m_moveSpeed;
    int m_score;
    int m_coinCount;
    bool m_isInvincible;
    float m_invincibilityTimer;
    int m_lives;

    bool m_isRunning;
    bool m_isSkidding;
    bool m_wasJumpPressed;
    bool m_pendingFixtureRebuild = false;
    bool m_pendingPowerDown = false;
    sf::Vector2f m_respawnPosition;

    float m_inputDirX = 0.0f;
    bool m_jumpRequested = false;
};
