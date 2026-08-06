/**
 * @file InputState.cpp
 * @author TV5 (Truyen)
 * @brief Event-driven keyboard state implementation.
 */

#include "patterns/InputState.h"

#include <algorithm>

void InputState::beginFrame() {
    std::fill(m_pressedThisFrame.begin(), m_pressedThisFrame.end(), false);
    std::fill(m_releasedThisFrame.begin(), m_releasedThisFrame.end(), false);
}

void InputState::handleEvent(const sf::Event& event) {
    if (event.is<sf::Event::FocusLost>()) {
        clear();
        return;
    }

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (!isValidKey(keyPressed->code)) {
            return;
        }

        const std::size_t index = toIndex(keyPressed->code);
        if (!m_held[index]) {
            m_held[index] = true;
            m_pressedThisFrame[index] = true;
            m_pressOrder[index] = ++m_nextPressOrder;
        }
        return;
    }

    if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()) {
        if (!isValidKey(keyReleased->code)) {
            return;
        }

        const std::size_t index = toIndex(keyReleased->code);
        if (m_held[index]) {
            m_held[index] = false;
            m_releasedThisFrame[index] = true;
        }
    }
}

bool InputState::isHeld(sf::Keyboard::Key key) const {
    return isValidKey(key) && m_held[toIndex(key)];
}

bool InputState::wasPressed(sf::Keyboard::Key key) const {
    return isValidKey(key) && m_pressedThisFrame[toIndex(key)];
}

bool InputState::wasReleased(sf::Keyboard::Key key) const {
    return isValidKey(key) && m_releasedThisFrame[toIndex(key)];
}

bool InputState::isActiveThisFrame(sf::Keyboard::Key key) const {
    return isHeld(key) || wasPressed(key);
}

std::uint64_t InputState::getPressOrder(sf::Keyboard::Key key) const {
    return isValidKey(key) ? m_pressOrder[toIndex(key)] : 0;
}

void InputState::clear() {
    std::fill(m_held.begin(), m_held.end(), false);
    std::fill(m_pressedThisFrame.begin(), m_pressedThisFrame.end(), false);
    std::fill(m_releasedThisFrame.begin(), m_releasedThisFrame.end(), false);
    std::fill(m_pressOrder.begin(), m_pressOrder.end(), 0);
}

bool InputState::isValidKey(sf::Keyboard::Key key) {
    const int keyValue = static_cast<int>(key);
    return keyValue >= 0 && keyValue < static_cast<int>(sf::Keyboard::KeyCount);
}

std::size_t InputState::toIndex(sf::Keyboard::Key key) {
    return static_cast<std::size_t>(key);
}
