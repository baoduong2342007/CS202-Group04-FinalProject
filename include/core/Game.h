/**
 * @file Game.h
 * @author TV2
 * @brief Game loop, window management and event handling
 * @note Week 1 — SFML 3.0.0 setup
 */
#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>

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
};

#endif