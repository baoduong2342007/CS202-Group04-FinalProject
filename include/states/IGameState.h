/**
 * @file IGameState.h
 * @author TV1 (Dương)
 * @brief Interface for game states (State Pattern)
 * @note Sprint 6 — locked interface: onEnter/onExit/onPause/onResume lifecycle
 */

#pragma once

#include <SFML/Graphics.hpp>

class InputState;

// ============================================================
// PATTERN: State
// Reason: Allows the game to switch between Menu, Play, Pause, etc. without monolithic conditionals
// ============================================================

class IGameState {
public:
    // 1. Constructor / Destructor
    virtual ~IGameState() = default;

    // 2. Lifecycle methods (Sprint 6 locked contract)
    virtual void onEnter() = 0;
    virtual void onExit() = 0;
    virtual void onPause() {}
    virtual void onResume() {}

    // 3. Frame methods
    virtual void processEvents(const sf::Event& event) = 0;
    virtual void processInput(const InputState& inputState) = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderTarget& target) = 0;
    virtual bool isOverlay() const { return false; }
};