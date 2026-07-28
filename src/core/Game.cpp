/**
 * @file Game.cpp
 * @author TV2
 * @brief Game loop, window management and event handling
 * @note Week 2 — SFML 3.0.0 setup. Sprint 4 — integrated Level + PhysicsEngine
 */
#include "core/Game.h"
#include "physics/PhysicsEngine.h"
#include <iostream>

// --- No More Magic Numbers ---
const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;
const unsigned int FRAMERATE_LIMIT = 60;

Game::Game()
    : m_window(sf::VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}),
               "Super Mario - CS202 Group 04",
               sf::Style::Titlebar | sf::Style::Close)
{
    m_window.setFramerateLimit(FRAMERATE_LIMIT);

    // Task 1.1: Initialize Box2D physics world with downward gravity
    PhysicsEngine::getInstance().init(
        sf::Vector2f(0.f, 9.8f * PhysicsEngine::PPM));

    // Load Level 1 (TileMap + spawn entities via Factory)
    if (!m_level.loadFromFile("levels/level1.txt")) {
        std::cerr << "FATAL: Failed to load levels/level1.txt!"
                  << std::endl;
    }
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
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                m_window.close();
            }
        }
    }
}

void Game::update(float dt) {
    // Task 1.4: Game loop — the heart of the engine

    // 1. Handle player input
    //    (tạm thời gọi trực tiếp, TV5 sẽ refactor sang InputHandler — Task 3.2)
    m_level.getMario().handleInput();

    // 2. Step Box2D physics simulation
    PhysicsEngine::getInstance().update(dt);

    // 3. Update all entities (Mario, enemies, items) + camera follow
    m_level.update(dt);
}

void Game::render() {
    m_window.clear(sf::Color(100, 149, 237)); // Sky blue background

    // Level handles: camera setView → TileMap → entities → Mario
    m_level.render(m_window);

    // Reset view to default for HUD overlay (TV5 future — Task X.1)
    m_window.setView(m_window.getDefaultView());

    m_window.display();
}