/**
 * @file IMarioState.h
 * @author TV3 (Bảo)
 * @brief Interface for Mario power-up State Pattern
 * @note Design Pattern #4 — State Pattern for Mario
 */

#pragma once

#include <SFML/Graphics.hpp>

// Forward declaration
class Mario;

enum class MarioState;

class IMarioState {
public:
    virtual ~IMarioState() = default;

    /// State lifecycle callbacks
    virtual void onEnter(Mario& mario) = 0;
    virtual void onExit(Mario& mario) = 0;

    /// Update state-specific logic (e.g. animation frames, state timers)
    virtual void update(Mario& mario, float dt) = 0;

    /// State attributes
    virtual MarioState getStateType() const = 0;
    virtual sf::Vector2f getHitboxSize() const = 0;
    virtual bool canShootFireBall() const = 0;
    virtual bool canBreakBricks() const = 0;
};
