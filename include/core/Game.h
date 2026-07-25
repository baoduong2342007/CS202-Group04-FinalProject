/**
 * @file Game.h
 * @author TV2
 * @brief Game loop, window management and event handling
 * @note Week 2 — SFML 3.0.0 setup
 */

#pragma once

#include <SFML/Graphics.hpp>
#include "level/TileMap.h"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float dt); // Changed from sf::Time to float
    void render();

    sf::RenderWindow m_window;
    sf::CircleShape m_shape;
    TileMap m_tileMap;
};