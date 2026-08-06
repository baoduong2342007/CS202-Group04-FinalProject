/**
 * @file GameManager.h
 * @author TV1 (Dương)
 * @brief Singleton manager for game states
 * @note Sprint 6 — deferred state operations (S6-TV1-16)
 */

#pragma once

#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>
#include "states/IGameState.h"

class InputState;

// ============================================================
// PATTERN: Singleton
// Reason: Ensures only one Game state manager controls the application flow
// ============================================================

class GameManager {
public:
    // 3. Public methods
    static GameManager& getInstance();

    // State operations are deferred to a safe point at the end of update()
    // so a state is never destroyed while it is on the call stack.
    void changeState(std::unique_ptr<IGameState> newState);
    void pushState(std::unique_ptr<IGameState> newState);
    void popState();

    void processEvents(const sf::Event& event);
    void processInput(const InputState& inputState);
    void update(float dt);
    void render(sf::RenderWindow& window);

private:
    // 1. Constructor / Destructor
    GameManager() = default;
    ~GameManager() = default;

    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    // 4. Pending operation types
    enum class StateOp {
        CHANGE,
        PUSH,
        POP
    };

    struct PendingOp {
        StateOp op;
        std::unique_ptr<IGameState> state; // only used for CHANGE/PUSH
    };

    // 5. Private methods
    void processPendingOps();

    // 6. Private members
    std::unique_ptr<IGameState> m_currentState;
    std::unique_ptr<IGameState> m_previousState;
    std::vector<PendingOp> m_pendingOps;
};