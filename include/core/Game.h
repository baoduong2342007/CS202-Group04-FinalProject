/**
 * @file Game.h
 * @author TV2
 * @brief Game loop, window management and event handling
 * @note Sprint 4 fix — delegates update and render to GameManager
 */

#pragma once

#include <SFML/Graphics.hpp>

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();

    sf::RenderWindow m_window;
};