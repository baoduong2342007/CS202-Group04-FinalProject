/**
 * @file Game.h
 * @author TV2
 * @brief Game loop, window management and event handling
 * @note Sprint 4 fix — delegates update and render to GameManager
 */

#pragma once

#include <SFML/Graphics.hpp>

#include "patterns/InputState.h"

class Game {
public:
    /**
     * @brief Constructs the Game object, initializing the window, render texture, and initial state.
     */
    Game();

    /**
     * @brief The main game loop. Handles events, updates, and rendering continuously until the window is closed.
     */
    void run();

private:
    /**
     * @brief Processes window events and input, delegating them to the GameManager.
     */
    void processEvents();

    /**
     * @brief Updates the game state and logic.
     * @param dt The time elapsed since the last update frame in seconds.
     */
    void update(float dt);

    /**
     * @brief Renders the game to the logical texture, then scales and draws it to the window.
     */
    void render();

    sf::RenderWindow m_window;
    sf::RenderTexture m_renderTexture;
    InputState m_inputState;
};
