/**
 * @file GameManager.cpp
 * @author TV1 (Dương)
 * @brief Implementation of GameManager
 */

#include "core/GameManager.h"
#include "states/PauseState.h"

GameManager& GameManager::getInstance() {
    static GameManager instance;
    return instance;
}

void GameManager::changeState(std::unique_ptr<IGameState> newState) {
    m_nextState = std::move(newState);
}

void GameManager::pushState(std::unique_ptr<IGameState> newState) {
    m_previousState = std::move(m_currentState);
    m_currentState = std::move(newState);
    m_currentState->onEnter();
}

void GameManager::popState() {
    m_isPopping = true;
}

void GameManager::update(float dt) {
    if (m_isPopping) {
        m_isPopping = false;
        if (m_currentState) {
            m_currentState->onExit();
        }
        m_currentState = std::move(m_previousState);
        // Do not call onEnter() again for resumed state, or call onResume() if it exists.
        // For now, PlayState doesn't need re-init on resume.
    } else if (m_nextState) {
        if (m_currentState) {
            m_currentState->onExit();
        }
        // Destroy previous state if any, since this is a hard change
        m_previousState.reset(); 
        
        m_currentState = std::move(m_nextState);
        m_currentState->onEnter();
    }

    if (m_currentState) {
        m_currentState->update(dt);
    }
}

void GameManager::processEvents(const sf::Event& event) {
    if (m_currentState) {
        m_currentState->processEvents(event);
    }
}

void GameManager::render(sf::RenderWindow& window) {
    if (m_currentState) {
        // If the current state is PauseState, render the previous state first
        if (dynamic_cast<PauseState*>(m_currentState.get()) && m_previousState) {
            m_previousState->render(window);
        }
        m_currentState->render(window);
    }
}
