/**
 * @file Podoboo.h
 * @brief Lava bubble that leaps out of the lava and falls back in
 * @note Enemy expansion - SMB1-accurate: completely invincible (star, fire,
 *       and shell all pass it by); contact always harms Mario.
 */

#pragma once

#include "core/LevelCatalog.h"
#include "entities/Enemy.h"

class Podoboo : public Enemy {
public:
    Podoboo(const sf::Vector2f& position,
            b2World* world,
            LevelTheme theme = LevelTheme::CASTLE);
    ~Podoboo() override = default;

    void update(float dt) override;

    void patrol() override;
    void onStomp() override;
    void onWallCollision() override;
    void onFireHit() override;

    bool isPodoboo() const override { return true; }
    bool canBeStomped() const override { return false; }
    bool isIndestructible() const override { return true; }
    bool isDying() const override {
        return !isActive();
    }

    /// True while the bubble is above the lava line.
    bool isAirborne() const { return m_phase == Phase::FLYING; }

private:
    enum class Phase {
        FLYING,          ///< Rising and falling along the manual parabola
        SUBMERGED_WAITING ///< Hidden below the lava line before the next leap
    };

    Phase m_phase{Phase::SUBMERGED_WAITING};
    float m_timer{0.f};
    float m_lavaY{0.f};
    float m_velocityY{0.f};

    static constexpr float LAUNCH_SPEED = 554.f; ///< px/s upward (~192 px peak)
    static constexpr float GRAVITY = 800.f;      ///< px/s^2 downward
    static constexpr float WAIT_TIME = 1.2f;     ///< Submerged pause between leaps
};
