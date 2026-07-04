#include "core/Game.h"
#include <iostream>

Game::Game() 
    // Initialize window in the initializer list
    : window(sf::VideoMode({1280, 720}), "Super Mario - Test SFML", sf::Style::Titlebar | sf::Style::Close),
      shape(100.f) // Initialize circle radius
{
    window.setFramerateLimit(60);

    // Setup the test circle
    shape.setFillColor(sf::Color(46, 204, 113)); 
    shape.setPosition({
        (1280 / 2.f) - 100.f,
        (720 / 2.f) - 100.f
    });
}

void Game::run() {
    std::cout << "Starting SFML window. Press ESC or close the window to exit." << std::endl;
    
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        
        processEvents();
        update(deltaTime);
        render();
    }

    std::cout << "Window closed successfully." << std::endl;
}

void Game::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                window.close();
            }
        }
    }
}

void Game::update(sf::Time deltaTime) {
    // Future physics and logic updates will go here
}

void Game::render() {
    window.clear(sf::Color(30, 30, 30));
    window.draw(shape);
    window.display();
}