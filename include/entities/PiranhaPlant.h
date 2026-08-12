/**
 * @file PiranhaPlant.h
 * @author TV4 & TV1
 * @brief Piranha Plant pipe enemy with emergence cycle and proximity guard
 * @note Week 7 implementation for Piranha Plant pipe enemy
 */

#pragma once

#include "core/LevelCatalog.h"
#include "entities/Enemy.h"

class Mario;

class PiranhaPlant : public Enemy {
public:
    enum class State {
        EMERGING,
        WAITING_TOP,
        RETRACTING,
        WAITING_BOTTOM
    };

    PiranhaPlant(const sf::Vector2f& position,
                 b2World* world,
                 LevelTheme theme = LevelTheme::OVERWORLD);
    ~PiranhaPlant() override = default;

    void update(float dt) override;

    void patrol() override;
    void onStomp() override;
    void onWallCollision() override;
    void onFireHit() override;

    bool isPiranhaPlant() const override { return true; }
    bool isDying() const override {
        return m_isFlippedDead || isDead() || !isActive();
    }

    State getState() const { return m_state; }

    /// Update Mario position to check proximity before emerging from pipe
    void updateMarioProximity(const sf::Vector2f& marioPos);

private:
    void initAnimations(LevelTheme theme);
    void updateState(float dt);
    bool isMarioNearPipe() const { return m_isMarioNear; }

    State m_state{State::WAITING_BOTTOM};
    sf::Vector2f m_basePosition;
    float m_stateTimer{0.f};
    bool m_isFlippedDead{false};
    float m_flipTimer{0.f};
    bool m_pendingDisablePhysics{false};
    bool m_isMarioNear{false};

    // --- NES Scale Factors (Original NES 16x24 sprite @ 2.0x scale -> 32x48px) ---
    static constexpr float TRAVEL_DISTANCE = 64.f;  // Pixels to move up out of pipe (64px travel = 48px full plant height above pipe rim!)
    static constexpr float MOVE_SPEED = 64.f;       // Pixels per second (1.0s emergence time)
    static constexpr float WAIT_TIME = 1.5f;        // Pause time at top and bottom inside pipe
    static constexpr float PROXIMITY_RADIUS = 20.f; // 20px guard (only holds when Mario is directly on top of pipe, emerges as Mario approaches!)
};
