/**
 * @file InputHandler.h
 * @author TV5
 * @brief Maps keyboard keys to Command objects for flexible input handling
 * @note Week 2 — uses Command pattern (ICommand) to decouple keys from actions
 */

#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

// 1. Standard library
#include <memory>
#include <unordered_map>

// 2. SFML
#include <SFML/Window/Keyboard.hpp>

// 3. Project headers
#include "patterns/ICommand.h"

// ============================================================
// PATTERN: Command (InputHandler component)
// Reason: maps sf::Keyboard::Key → ICommand*; allows rebinding
//         keys at runtime without touching game logic
// ============================================================

class InputHandler {
public:
    // 1. Constructor / Destructor
    InputHandler() = default;
    ~InputHandler() = default;

    // 3. Public methods
    /// Bind a key to a command (takes ownership via unique_ptr)
    void bindKey(sf::Keyboard::Key key, std::unique_ptr<ICommand> command);

    /// Remove the binding for a key
    void unbindKey(sf::Keyboard::Key key);

    /// Poll all bound keys and execute commands for pressed keys
    void handleInput() const;

    // 4. Getters
    /// Retrieve the command bound to a key (nullptr if unbound).
    /// Returns a non-owning pointer — caller must NOT delete it.
    ICommand* getAction(sf::Keyboard::Key key) const;

    /// Check whether a key currently has a command bound to it
    bool isBound(sf::Keyboard::Key key) const;

private:
    // 6. Private members
    std::unordered_map<sf::Keyboard::Key, std::unique_ptr<ICommand>> m_keyBindings;
};

#endif // INPUTHANDLER_H
