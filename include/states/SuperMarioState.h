/**
 * @file SuperMarioState.h
 * @author TV3 (Bao)
 * @brief Concrete Super Mario power-up state (super body, breaks bricks)
 */

#pragma once

#include "states/IMarioState.h"
#include "entities/Mario.h"

class SuperMarioState : public IMarioState {
public:
    SuperMarioState() = default;
    ~SuperMarioState() override = default;

    void onEnter(Mario& mario) override;
    void onExit(Mario& mario) override;
    void update(Mario& mario, float dt) override;

    MarioState getStateType() const override { return MarioState::SUPER; }
    sf::Vector2f getHitboxSize() const override { return sf::Vector2f(32.f, 64.f); }
    bool canShootFireBall() const override { return false; }
    bool canBreakBricks() const override { return true; }
};
