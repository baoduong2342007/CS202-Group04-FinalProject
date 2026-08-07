/**
 * @file FireMarioState.h
 * @author TV3 (Bảo)
 * @brief Concrete Fire Mario power-up state (32x64 hitbox, shoots fireballs)
 */

#pragma once

#include "states/IMarioState.h"
#include "entities/Mario.h"

class FireMarioState : public IMarioState {
public:
    FireMarioState() = default;
    ~FireMarioState() override = default;

    void onEnter(Mario& mario) override;
    void onExit(Mario& mario) override;
    void update(Mario& mario, float dt) override;

    MarioState getStateType() const override { return MarioState::FIRE; }
    sf::Vector2f getHitboxSize() const override { return sf::Vector2f(32.f, 64.f); }
    bool canShootFireBall() const override { return true; }
    bool canBreakBricks() const override { return true; }
};
