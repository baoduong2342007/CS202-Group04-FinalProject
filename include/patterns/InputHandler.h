/**
 * @file InputHandler.h
 * @author TV5
 * @brief Maps keyboard keys to Command objects for flexible input handling
 * @note Week 2 — uses Command pattern (ICommand) to decouple keys from actions
 */

#pragma once

// 1. Standard library
#include <memory>
#include <unordered_map>

// 2. SFML
#include <SFML/Window/Keyboard.hpp>

// 3. Project headers
#include "patterns/ICommand.h"
#include "patterns/InputState.h"

// ============================================================
// PATTERN: Command (InputHandler component)
// Reason: maps sf::Keyboard::Key → ICommand*; allows rebinding
//         keys at runtime without touching game logic
// ============================================================

enum class InputTrigger {
    Pressed,
    Held
};

enum class InputGroup {
    None,
    Horizontal
};

class InputHandler {
public:
    // 1. Constructor / Destructor
    InputHandler() = default;
    ~InputHandler() = default;

    // 3. Public methods
    /// Bind a key to a command (takes ownership via unique_ptr)
    void bindKey(sf::Keyboard::Key key,
                 std::unique_ptr<ICommand> command,
                 InputTrigger trigger = InputTrigger::Held,
                 InputGroup group = InputGroup::None);

    /// Remove the binding for a key
    void unbindKey(sf::Keyboard::Key key);

    /// Clear all bound keys
    void clear();

    /// Dispatch commands from the event-driven input state.
    void handleInput(const InputState& inputState) const;

    // 4. Getters
    /// Retrieve the command bound to a key (nullptr if unbound).
    /// Returns a non-owning pointer — caller must NOT delete it.
    ICommand* getAction(sf::Keyboard::Key key) const;

    /// Check whether a key currently has a command bound to it
    bool isBound(sf::Keyboard::Key key) const;

private:
    struct Binding {
        std::unique_ptr<ICommand> command;
        InputTrigger trigger;
        InputGroup group;
    };

    // 6. Private members
    std::unordered_map<sf::Keyboard::Key, Binding> m_keyBindings;
};
