/**
 * @file GameManager.cpp
 * @author TV1 (Dương)
 * @brief Implementation of GameManager
 * @note Sprint 6 — deferred state operations (S6-TV1-16)
 */

#include "core/GameManager.h"

GameManager& GameManager::getInstance() {
    static GameManager instance;
    return instance;
}

void GameManager::changeState(std::unique_ptr<IGameState> newState) {
    m_pendingOps.push_back({StateOp::CHANGE, std::move(newState)});
}

void GameManager::pushState(std::unique_ptr<IGameState> newState) {
    m_pendingOps.push_back({StateOp::PUSH, std::move(newState)});
}

void GameManager::popState() {
    m_pendingOps.push_back({StateOp::POP, nullptr});
}

void GameManager::processPendingOps() {
    // Process all queued operations in order. This runs at a safe point
    // (end of update) so no state is destroyed while it is on the call stack.
    for (auto& op : m_pendingOps) {
        switch (op.op) {
            case StateOp::CHANGE: {
                if (m_currentState) {
                    m_currentState->onExit();
                }
                // Hard change: previous state is no longer needed
                m_previousState.reset();
                m_currentState = std::move(op.state);
                if (m_currentState) {
                    m_currentState->onEnter();
                }
                break;
            }
            case StateOp::PUSH: {
                if (m_currentState) {
                    m_currentState->onPause();
                }
                m_previousState = std::move(m_currentState);
                m_currentState = std::move(op.state);
                if (m_currentState) {
                    m_currentState->onEnter();
                }
                break;
            }
            case StateOp::POP: {
                if (m_currentState) {
                    m_currentState->onExit();
                }
                m_currentState = std::move(m_previousState);
                if (m_currentState) {
                    m_currentState->onResume();
                }
                break;
            }
        }
    }
    m_pendingOps.clear();
}

void GameManager::update(float dt) {
    if (m_currentState) {
        m_currentState->update(dt);
    }

    // Process deferred state operations at the safe point (end of update)
    processPendingOps();
}

void GameManager::processEvents(const sf::Event& event) {
    if (m_currentState) {
        m_currentState->processEvents(event);
    }
}

void GameManager::processInput(const InputState& inputState) {
    if (m_currentState) {
        m_currentState->processInput(inputState);
    }
}

void GameManager::render(sf::RenderWindow& window) {
    if (m_currentState) {
        // If the current state is an overlay (e.g., PauseState), render the previous state first
        if (m_currentState->isOverlay() && m_previousState) {
            m_previousState->render(window);
        }
        m_currentState->render(window);
    }
}