/**
 * @file Paratroopa.h
 * @brief Winged Koopa variant: green hopper and red vertical flyer
 * @note Enemy expansion - SMB1-accurate: the first stomp only clips the
 *       wings (the enemy becomes a normal walking Koopa of its palette);
 *       the second stomp starts the shared shell state machine.
 */

#pragma once

#include "core/LevelCatalog.h"
#include "entities/Koopa.h"

enum class ParatroopaMode {
    HOP,           ///< Green: bounces along the ground
    FLY_VERTICAL   ///< Red: patrols a vertical corridor around its spawn
};

class Paratroopa : public Koopa {
public:
    Paratroopa(const sf::Vector2f& position,
               b2World* world,
               LevelTheme theme,
               ParatroopaMode mode = ParatroopaMode::HOP);
    ~Paratroopa() override = default;

    void update(float dt) override;
    void patrol() override;
    void onStomp() override;
    void onWallCollision() override;

    bool hasWings() const { return m_hasWings; }
    ParatroopaMode getMode() const { return m_mode; }
    bool isParatroopa() const override { return true; }

private:
    /// SMB1 wing-clip transition: gravity returns, the walk animation takes
    /// over, and the stomp latch is re-armed for the shell stomp to come.
    void clipWings();

    ParatroopaMode m_mode;
    bool m_hasWings = true;
    float m_anchorY = 0.f;
    bool m_flyingUp = true;
    float m_rearmTimer = 0.f;
};
