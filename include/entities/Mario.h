/**
 * @file Mario.h
 * @author TV3 (Bảo)
 * @brief Mario player character class with physics, power-up states, and lives management
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

protected:
    // 5. Protected methods
    void rebuildFixture();

    // 6. Protected / Private members
    MarioState m_marioState;
    float m_jumpForce;
    float m_moveSpeed;
    int m_lives;
};
