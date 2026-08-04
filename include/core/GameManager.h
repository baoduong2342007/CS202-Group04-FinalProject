/**
 * @file GameManager.h
 * @author TV1 (Dương)
 * @brief Singleton manager for game states
 */

#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "states/IGameState.h"

// ============================================================
// PATTERN: Singleton
// Reason: Ensures only one Game state manager controls the application flow
// ============================================================

class GameManager {
public:
    // 3. Public methods
    static GameManager& getInstance();

    void changeState(std::unique_ptr<IGameState> newState);
    void pushState(std::unique_ptr<IGameState> newState);
    void popState();
    
    void processEvents(const sf::Event& event);
    void update(float dt);
    void render(sf::RenderWindow& window);

private:
    // 1. Constructor / Destructor
    GameManager() = default;
    ~GameManager() = default;
    
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    // 6. Private members
    std::unique_ptr<IGameState> m_currentState;
    std::unique_ptr<IGameState> m_nextState;
    std::unique_ptr<IGameState> m_previousState;
    bool m_isPopping = false;
};
