/**
 * @file Mario.h
 * @author TV3 (Bảo)
 * @brief Mario player character class with authentic NES movement physics and states
 * @note Week 7 update
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
    void handleInput();
    void jump();
    void moveLeft();
    void moveRight();
    void stopMoving();
    void powerUp(MarioState state);
    void powerDown();
    void loseLife();
    void respawn(const sf::Vector2f& spawnPosition);

    // 4. Getters / Setters
    MarioState getMarioState() const;
    void setMarioState(MarioState state);
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
    int m_lives;

    bool m_isRunning;
    bool m_isSkidding;
    bool m_wasJumpPressed;
};
