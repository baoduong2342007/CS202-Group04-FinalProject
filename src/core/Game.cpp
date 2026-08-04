/**
 * @file Game.cpp
 * @author TV2
 * @brief Game loop, window management and event handling
 * @note Sprint 4 fix — delegates logic to GameManager (State Pattern)
 */
#include "core/Game.h"
#include <iostream>
#include "core/GameManager.h"
#include "states/MenuState.h"

namespace {
constexpr unsigned int SCREEN_WIDTH = 1280;
constexpr unsigned int SCREEN_HEIGHT = 720;
constexpr unsigned int FRAMERATE_LIMIT = 60;
} // namespace

Game::Game()
    : m_window(sf::VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}),
               "Super Mario - CS202 Group 04",
               sf::Style::Titlebar | sf::Style::Close)
{
    m_window.setFramerateLimit(FRAMERATE_LIMIT);

    // Physics world is now owned and initialized by Level.

    // Transition to MenuState as the initial state
    GameManager::getInstance().changeState(std::make_unique<MenuState>());
}
void Game::run() {
#ifdef DEBUG
    std::cout << "Starting SFML window. Press ESC or close the window to exit."
              << std::endl;
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
        // Delegate event to current state
        GameManager::getInstance().processEvents(*event);
    }
}

void Game::update(float dt) {
    // Delegate update to GameManager
    GameManager::getInstance().update(dt);
}
void Game::render() {
    m_window.clear(sf::Color(100, 149, 237)); // Sky blue background

    // Delegate rendering to GameManager
    GameManager::getInstance().render(m_window);
    
    m_window.display();
}