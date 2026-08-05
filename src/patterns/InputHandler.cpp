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
                           std::unique_ptr<ICommand> command,
                           InputTrigger trigger,
                           InputGroup group) {
    m_keyBindings[key] = Binding{std::move(command), trigger, group};
}

void InputHandler::unbindKey(sf::Keyboard::Key key) {
    m_keyBindings.erase(key);
}

void InputHandler::clear() {
    m_keyBindings.clear();
}

void InputHandler::handleInput(const InputState& inputState) const {
    for (const auto& [key, binding] : m_keyBindings) {
        if (binding.trigger == InputTrigger::Pressed && inputState.wasPressed(key)) {
            binding.command->execute();
        }
    }

    for (const auto& [key, binding] : m_keyBindings) {
        if (binding.trigger == InputTrigger::Held && binding.group == InputGroup::None &&
            inputState.isActiveThisFrame(key)) {
            binding.command->execute();
        }
    }

    const Binding* horizontalBinding = nullptr;
    std::uint64_t latestPressOrder = 0;

    for (const auto& [key, binding] : m_keyBindings) {
        if (binding.trigger != InputTrigger::Held || binding.group != InputGroup::Horizontal ||
            !inputState.isActiveThisFrame(key)) {
            continue;
        }

        const std::uint64_t pressOrder = inputState.getPressOrder(key);
        if (!horizontalBinding || pressOrder > latestPressOrder) {
            horizontalBinding = &binding;
            latestPressOrder = pressOrder;
        }
    }

    if (horizontalBinding) {
        horizontalBinding->command->execute();
    }
}

ICommand* InputHandler::getAction(sf::Keyboard::Key key) const {
    auto it = m_keyBindings.find(key);
    if (it != m_keyBindings.end()) {
        return it->second.command.get();
    }
    return nullptr;
}

bool InputHandler::isBound(sf::Keyboard::Key key) const {
    return m_keyBindings.count(key) > 0;
}
