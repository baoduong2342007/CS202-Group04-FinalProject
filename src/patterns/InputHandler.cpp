/**
 * @file InputHandler.cpp
 * @author TV5
 * @brief Maps keyboard keys to Command objects for flexible input handling
 * @note Week 2 — uses Command pattern (ICommand) to decouple keys from actions
 */

#include "patterns/InputHandler.h"

// ============================================================
// PATTERN: Command (InputHandler component)
// Reason: maps sf::Keyboard::Key → ICommand*; allows rebinding
//         keys at runtime without touching game logic
// ============================================================

void InputHandler::bindKey(sf::Keyboard::Key key,
                           std::unique_ptr<ICommand> command) {
    m_keyBindings[key] = std::move(command);
}

void InputHandler::unbindKey(sf::Keyboard::Key key) {
    m_keyBindings.erase(key);
}

void InputHandler::clear() {
    m_keyBindings.clear();
}

void InputHandler::handleInput() const {
    for (const auto& [key, command] : m_keyBindings) {
        if (sf::Keyboard::isKeyPressed(key)) {
            command->execute();
        }
    }
}

ICommand* InputHandler::getAction(sf::Keyboard::Key key) const {
    auto it = m_keyBindings.find(key);
    if (it != m_keyBindings.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool InputHandler::isBound(sf::Keyboard::Key key) const {
    return m_keyBindings.count(key) > 0;
}
