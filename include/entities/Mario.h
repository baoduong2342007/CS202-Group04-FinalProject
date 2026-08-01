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
    void powerUp(MarioState state);
    void powerDown();
    void addScore(int points);
    void setInvincible(float duration);
    void updateInvincibility(float dt);
    void loseLife();
    void respawn(const sf::Vector2f& spawnPosition);

    // 4. Getters / Setters
    MarioState getMarioState() const;
    void setMarioState(MarioState state);
    int getScore() const;
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
    bool m_isInvincible;
    float m_invincibilityTimer;
    int m_lives;

    bool m_isRunning;
    bool m_isSkidding;
    bool m_wasJumpPressed;
};
