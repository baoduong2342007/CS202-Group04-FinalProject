/**
 * @file InputState.h
 * @author TV5 (Truyen)
 * @brief Per-frame keyboard state built from SFML window events.
 */

#pragma once

#include <array>
#include <cstdint>

#include <SFML/Window/Event.hpp>

class InputState {
public:
    /// Clear one-frame edge flags before polling the next batch of window events.
    void beginFrame();

    /// Record keyboard and focus events received from the game window.
    void handleEvent(const sf::Event& event);

    bool isHeld(sf::Keyboard::Key key) const;
    bool wasPressed(sf::Keyboard::Key key) const;
    bool wasReleased(sf::Keyboard::Key key) const;

    /// A held key or one pressed during this frame. This preserves short taps.
    bool isActiveThisFrame(sf::Keyboard::Key key) const;

    /// Monotonically increasing order of the last physical press for a key.
    std::uint64_t getPressOrder(sf::Keyboard::Key key) const;

    /// Synchronize held key states with physical keyboard hardware to prevent stuck keys.
    void syncHardware();

    /// Forget all key states, used when the window loses focus.
    void clear();

private:
    static bool isValidKey(sf::Keyboard::Key key);
    static std::size_t toIndex(sf::Keyboard::Key key);

    std::array<bool, sf::Keyboard::KeyCount> m_held{};
    std::array<bool, sf::Keyboard::KeyCount> m_pressedThisFrame{};
    std::array<bool, sf::Keyboard::KeyCount> m_releasedThisFrame{};
    std::array<std::uint64_t, sf::Keyboard::KeyCount> m_pressOrder{};
    std::uint64_t m_nextPressOrder = 0;
};
