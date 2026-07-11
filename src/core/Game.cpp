/**
 * @file Game.cpp
 * @author TV2
 * @brief Game loop, window management and event handling
 * @note Week 2 — SFML 3.0.0 setup
 */
#include "core/Game.h"
#include <iostream>

// --- No More Magic Numbers ---
const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;
const unsigned int FRAMERATE_LIMIT = 60;
const float CIRCLE_RADIUS = 100.f;

Game::Game() 
    : m_window(sf::VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}), "Super Mario - Test SFML", sf::Style::Titlebar | sf::Style::Close),
      m_shape(CIRCLE_RADIUS) 
{
    m_window.setFramerateLimit(FRAMERATE_LIMIT);

    m_shape.setFillColor(sf::Color(46, 204, 113)); 
    m_shape.setPosition({
        (SCREEN_WIDTH / 2.f) - CIRCLE_RADIUS,
        (SCREEN_HEIGHT / 2.f) - CIRCLE_RADIUS
    });
}

void Game::run() {
#ifdef DEBUG
    std::cout << "Starting SFML window. Press ESC or close the window to exit." << std::endl;
#endif
    
    sf::Clock clock;
    while (m_window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        
        // Convert sf::Time to float (seconds) for the architecture
        float dt = deltaTime.asSeconds(); 
        
        processEvents();
        update(dt);
        render();
    }

#ifdef DEBUG
    std::cout << "Window closed successfully." << std::endl;
#endif
}

void Game::processEvents() {
    while (const std::optional event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                m_window.close();
            }
        }
    }
}

void Game::update(float dt) {
    // Suppress "unused parameter" compiler warning until we add real logic
    (void)dt; 
}

void Game::render() {
    m_window.clear(sf::Color(30, 30, 30));
    m_window.draw(m_shape);
    m_window.display();
}