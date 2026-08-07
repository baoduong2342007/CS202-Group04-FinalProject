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

    /// Number of states currently on the stack (used by regression tests).
    int stackDepth() const { return static_cast<int>(m_stateStack.size()); }

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
    void applyOp(PendingOp& op);
    void processPendingOps();
    IGameState* top() { return m_stateStack.empty() ? nullptr : m_stateStack.back().get(); }

    // 6. Private members
    // A true state stack. CHANGE resets the whole stack, PUSH overlays a state
    // on top (pausing the one below), POP removes the top and resumes the new one.
    std::vector<std::unique_ptr<IGameState>> m_stateStack;
    std::vector<PendingOp> m_pendingOps;
};