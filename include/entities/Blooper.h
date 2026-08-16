/**
 * @file Blooper.h
 * @brief Underwater squid enemy that drifts down toward Mario and rises again
 * @note Enemy expansion - SMB1-accurate: unstompable while swimming, killed
 *       by fireballs; zig-zag descent/rise cycle, no tile physics.
 */

#pragma once

#include "core/LevelCatalog.h"
#include "entities/Enemy.h"

class Blooper : public Enemy {
public:
    enum class State {
        DRIFT, ///< Tentacles extended, sinking diagonally toward Mario
        RISE   ///< Body contracted, thrusting upward
    };

    Blooper(const sf::Vector2f& position,
            b2World* world,
            LevelTheme theme = LevelTheme::UNDERWATER);
    ~Blooper() override = default;

    void update(float dt) override;

    void patrol() override;
    void onStomp() override;
    void onWallCollision() override;
    void onFireHit() override;

    bool isBlooper() const override { return true; }
    bool canBeStomped() const override { return false; }
    bool isDying() const override {
        return m_isFlippedDead || isDead() || !isActive();
    }

    State getState() const { return m_state; }

    /// Feed Mario's position so the drift phase can home in horizontally.
    void updateMarioPosition(const sf::Vector2f& marioPos);

private:
    State m_state{State::RISE};
    float m_stateTimer{0.f};
    sf::Vector2f m_marioPosition{0.f, 0.f};
    bool m_marioKnown{false};
    bool m_isFlippedDead{false};

    static constexpr float DRIFT_TIME = 1.2f;
    static constexpr float RISE_TIME = 1.0f;
    static constexpr float DRIFT_FALL_SPEED = 40.f;
    static constexpr float DRIFT_TRACK_SPEED = 30.f;
    static constexpr float RISE_SPEED = 50.f;
    static constexpr float PIT_CLEANUP_Y = 800.f;
};
