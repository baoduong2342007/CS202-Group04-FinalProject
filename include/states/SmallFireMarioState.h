/**
 * @file SmallFireMarioState.h
 * @brief Concrete Small Fire Mario state.
 */

#pragma once

#include "states/IMarioState.h"
#include "entities/Mario.h"

class SmallFireMarioState : public IMarioState {
public:
    SmallFireMarioState() = default;
    ~SmallFireMarioState() override = default;

    void onEnter(Mario& mario) override;
    void onExit(Mario& mario) override;
    void update(Mario& mario, float dt) override;

    MarioState getStateType() const override { return MarioState::FIRE_SMALL; }
    sf::Vector2f getHitboxSize() const override { return {32.f, 32.f}; }
    bool canShootFireBall() const override { return true; }
    bool canBreakBricks() const override { return false; }
};
