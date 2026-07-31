/**
 * @file GameManager.cpp
 * @author TV1 (Dương)
 * @brief Implementation of GameManager
 */

#include "core/GameManager.h"

GameManager& GameManager::getInstance() {
    static GameManager instance;
    return instance;
}

void GameManager::changeState(std::unique_ptr<IGameState> newState) {
    m_nextState = std::move(newState);
}

void GameManager::update(float dt) {
    if (m_nextState) {
        if (m_currentState) {
            m_currentState->onExit();
        }
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
        m_currentState->render(window);
    }
}
