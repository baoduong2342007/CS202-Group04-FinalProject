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

void GameManager::applyOp(PendingOp& op) {
    switch (op.op) {
        case StateOp::CHANGE: {
            // Hard change: tear down every state on the stack (calling each onExit),
            // then replace with the single new state.
            for (auto& st : m_stateStack) {
                if (st) st->onExit();
            }
            m_stateStack.clear();
            m_stateStack.push_back(std::move(op.state));
            if (top()) top()->onEnter();
            break;
        }
        case StateOp::PUSH: {
            // Overlay: pause the current top, then push the new state on top.
            if (top()) top()->onPause();
            m_stateStack.push_back(std::move(op.state));
            if (top()) top()->onEnter();
            break;
        }
        case StateOp::POP: {
            // Pop the top state (calling onExit) and resume the one below it.
            if (top()) top()->onExit();
            if (!m_stateStack.empty()) {
                m_stateStack.pop_back();
            }
            if (top()) top()->onResume();
            break;
        }
    }
}

void GameManager::processPendingOps() {
    // S6-TV1-16: snapshot the queue before applying ops. If a lifecycle callback
    // (onEnter/onExit/onResume) queues a new operation, that operation is appended
    // to the (now empty) m_pendingOps and deferred to the next safe point, instead of
    // invalidating the iteration over m_pendingOps mid-loop.
    std::vector<PendingOp> ops;
    ops.swap(m_pendingOps);
    for (auto& op : ops) {
        applyOp(op);
    }
}

void GameManager::update(float dt) {
    if (top()) {
        top()->update(dt);
    }

    // Process deferred state operations at the safe point (end of update)
    processPendingOps();
}

void GameManager::processEvents(const sf::Event& event) {
    if (top()) {
        top()->processEvents(event);
    }
}

void GameManager::processInput(const InputState& inputState) {
    if (top()) {
        top()->processInput(inputState);
    }
}

void GameManager::render(sf::RenderWindow& window) {
    if (m_stateStack.empty()) {
        return;
    }

    // If the current top is an overlay (e.g. PauseState), render the state below it
    // first so the world is visible behind the overlay.
    if (top()->isOverlay() && m_stateStack.size() >= 2) {
        m_stateStack[m_stateStack.size() - 2]->render(window);
    }
    top()->render(window);
}