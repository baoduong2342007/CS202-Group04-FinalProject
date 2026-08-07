/**
 * @file SmallMarioState.h
 * @author TV3 (Bảo)
 * @brief Concrete Small Mario power-up state (32x32 hitbox, no fireball shooting)
 */

#pragma once

#include "states/IMarioState.h"
#include "entities/Mario.h"

class SmallMarioState : public IMarioState {
public:
    SmallMarioState() = default;
    ~SmallMarioState() override = default;

    void onEnter(Mario& mario) override;
    void onExit(Mario& mario) override;
    void update(Mario& mario, float dt) override;

    MarioState getStateType() const override { return MarioState::SMALL; }
    sf::Vector2f getHitboxSize() const override { return sf::Vector2f(32.f, 32.f); }
    bool canShootFireBall() const override { return false; }
    bool canBreakBricks() const override { return false; }
};
