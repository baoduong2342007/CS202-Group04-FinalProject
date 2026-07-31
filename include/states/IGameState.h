/**
 * @file IGameState.h
 * @author TV1 (Dương)
 * @brief Interface for game states (State Pattern)
 */

#pragma once

#include <SFML/Graphics.hpp>

// ============================================================
// PATTERN: State
// Reason: Allows the game to switch between Menu, Play, Pause, etc. without monolithic conditionals
// ============================================================

class IGameState {
public:
    // 1. Constructor / Destructor
    virtual ~IGameState() = default;

    // 2. Public methods
    virtual void onEnter() = 0;
    virtual void onExit() = 0;
    
    virtual void processEvents(const sf::Event& event) = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
};
