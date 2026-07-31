/**
 * @file Mario.h
 * @author TV3 (Bảo)
 * @brief Mario player class with physics and states
 * @note Week 1 skeleton setup
 */

#pragma once

#include "entities/Character.h"

// Define Mario's power-up states
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
    void handleInput(); // DEPRECATED — use InputHandler instead
    void jump();
    void moveLeft();
    void moveRight();
    void stopMoving();
    void powerUp(MarioState state);
    void powerDown();

    // 4. Getters / Setters
    MarioState getMarioState() const;
    void setMarioState(MarioState state);

protected:
    // 5. Protected members
    MarioState m_marioState;
    float m_jumpForce;
    float m_moveSpeed;
};
