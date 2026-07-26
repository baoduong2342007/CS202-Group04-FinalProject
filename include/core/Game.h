/**
 * @file Game.h
 * @author TV2
 * @brief Game loop, window management and event handling
 * @note Week 2 — SFML 3.0.0 setup. Sprint 4 — integrated Level + PhysicsEngine
 */

#pragma once

#include <SFML/Graphics.hpp>
#include "level/Level.h"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();

    sf::RenderWindow m_window;
    Level m_level;
};