/**
 * @file SuperFireMarioState.h
 * @brief Concrete Super Fire Mario state.
 */

#pragma once

#include "states/IMarioState.h"
#include "entities/Mario.h"

class SuperFireMarioState : public IMarioState {
public:
    SuperFireMarioState() = default;
    ~SuperFireMarioState() override = default;

    void onEnter(Mario& mario) override;
    void onExit(Mario& mario) override;
    void update(Mario& mario, float dt) override;

    MarioState getStateType() const override { return MarioState::FIRE_SUPER; }
    sf::Vector2f getHitboxSize() const override { return {32.f, 64.f}; }
    bool canShootFireBall() const override { return true; }
    bool canBreakBricks() const override { return true; }
};
