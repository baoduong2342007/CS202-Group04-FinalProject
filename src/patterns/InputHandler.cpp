/**
 * @file InputHandler.cpp
 * @author TV5
 * @brief Maps keyboard keys to Command objects for flexible input handling
 * @note Week 2 — uses Command pattern (ICommand) to decouple keys from actions
 */

#include "patterns/InputHandler.h"

#include <algorithm>

// ============================================================
// PATTERN: Command (InputHandler component)
// Reason: maps sf::Keyboard::Key → ICommand*; allows rebinding
//         keys at runtime without touching game logic
// ============================================================

void InputHandler::bindKey(sf::Keyboard::Key key,
                           std::unique_ptr<ICommand> command,
                           InputTrigger trigger,
                           InputGroup group) {
    auto& bindings = m_keyBindings[key];
    const auto existing = std::find_if(
        bindings.begin(),
        bindings.end(),
        [trigger, group](const Binding& binding) {
            return binding.trigger == trigger && binding.group == group;
        });

    Binding replacement{std::move(command), trigger, group};
    if (existing != bindings.end()) {
        *existing = std::move(replacement);
    } else {
        bindings.push_back(std::move(replacement));
    }
}

void InputHandler::unbindKey(sf::Keyboard::Key key) {
    m_keyBindings.erase(key);
}

void InputHandler::clear() {
    m_keyBindings.clear();
}

void InputHandler::handleInput(const InputState& inputState, bool gameplayEnabled) const {
    if (!gameplayEnabled) {
        return;
    }

    for (const auto& [key, bindings] : m_keyBindings) {
        for (const auto& binding : bindings) {
            if (!binding.command) {
                continue;
            }

            if (binding.trigger == InputTrigger::Pressed && inputState.wasPressed(key)) {
                binding.command->execute();
            } else if (binding.trigger == InputTrigger::Released && inputState.wasReleased(key)) {
                binding.command->execute();
            }
        }
    }

    for (const auto& [key, bindings] : m_keyBindings) {
        for (const auto& binding : bindings) {
            if (binding.trigger == InputTrigger::Held &&
                binding.group == InputGroup::None &&
                inputState.isActiveThisFrame(key) && binding.command) {
                binding.command->execute();
            }
        }
    }

    const Binding* horizontalBinding = nullptr;
    std::uint64_t latestPressOrder = 0;

    for (const auto& [key, bindings] : m_keyBindings) {
        for (const auto& binding : bindings) {
            if (binding.trigger != InputTrigger::Held ||
                binding.group != InputGroup::Horizontal ||
                !inputState.isActiveThisFrame(key) || !binding.command) {
                continue;
            }

            const std::uint64_t pressOrder = inputState.getPressOrder(key);
            if (!horizontalBinding || pressOrder > latestPressOrder) {
                horizontalBinding = &binding;
                latestPressOrder = pressOrder;
            }
        }
    }

    if (horizontalBinding) {
        horizontalBinding->command->execute();
    }
}

ICommand* InputHandler::getAction(sf::Keyboard::Key key) const {
    auto it = m_keyBindings.find(key);
    if (it != m_keyBindings.end() && !it->second.empty()) {
        return it->second.front().command.get();
    }
    return nullptr;
}

bool InputHandler::isBound(sf::Keyboard::Key key) const {
    const auto it = m_keyBindings.find(key);
    return it != m_keyBindings.end() && !it->second.empty();
}
