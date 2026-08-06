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
    void addCoin();
    std::unique_ptr<FireBall> shootFireBall(b2World* world);
    void setInvincible(float duration);
    void updateInvincibility(float dt);
    void loseLife();
    void respawn(const sf::Vector2f& spawnPosition);
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
    bool isInvincible() const;
    bool canShootFireBall() const;
    int getLives() const;
    void setLives(int lives);

    bool isRunning() const;
    bool isSkidding() const;
    bool isDying() const;
    bool isTransforming() const { return m_isTransforming; }

protected:
    // 5. Protected methods
    void rebuildFixture();
    void applyMovementPhysics(float dt, float inputDirX, bool isRunningInput, bool jumpKeyPressed, bool jumpKeyReleased);
    void applyGroundPhysics(float dt, float inputDirX, bool isRunningInput, bool jumpKeyPressed, float& currentVy, float& newVx, float targetMaxSpeed);
    void applyAirPhysics(float dt, float inputDirX, bool jumpKeyReleased, float& currentVy, float& newVx, float targetMaxSpeed);

    // 6. Protected / Private members
    MarioState m_marioState;
    std::unique_ptr<class IMarioState> m_statePattern;
    float m_jumpForce;
    float m_moveSpeed;
    int m_score;
    int m_coinCount;
    bool m_isInvincible;
    float m_invincibilityTimer;
    int m_lives;

    bool m_isDying;
    float m_deathTimer;
    bool m_isRunning;
    bool m_isSkidding;
    bool m_wasJumpPressed;

    // Transformation effect
    bool m_isTransforming;
    float m_transformTimer;

    // Fixture Caching
    b2Fixture* m_smallFixture = nullptr;
    b2Fixture* m_superFixture = nullptr;
};

