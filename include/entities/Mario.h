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

    // 4. Getters / Setters
    MarioState getMarioState() const;
    void setMarioState(MarioState state);
    int getScore() const;

protected:
    // 5. Protected members
    MarioState m_marioState;
    float m_jumpForce;
    float m_moveSpeed;
    int m_score;
};
