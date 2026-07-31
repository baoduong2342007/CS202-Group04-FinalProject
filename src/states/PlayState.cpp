/**
 * @file PlayState.cpp
 * @author TV1 (Dương)
 * @brief Play state implementation (handles the main gameplay loop)
 */

#include "states/PlayState.h"
#include <iostream>
#include "patterns/EventBus.h"
#include "patterns/JumpCommand.h"
#include "patterns/MoveLeftCommand.h"
#include "patterns/MoveRightCommand.h"

PlayState::PlayState() {
    // Load Level 1
    if (!m_level.loadFromFile("levels/level1.txt")) {
        std::cerr << "FATAL: Failed to load levels/level1.txt!\n";
    }

    // Bind commands to InputHandler
    m_inputHandler.bindKey(sf::Keyboard::Key::W, std::make_unique<JumpCommand>(&m_level.getMario()));
    m_inputHandler.bindKey(sf::Keyboard::Key::Up, std::make_unique<JumpCommand>(&m_level.getMario()));
    m_inputHandler.bindKey(sf::Keyboard::Key::Space, std::make_unique<JumpCommand>(&m_level.getMario()));
    
    m_inputHandler.bindKey(sf::Keyboard::Key::A, std::make_unique<MoveLeftCommand>(&m_level.getMario()));
    m_inputHandler.bindKey(sf::Keyboard::Key::Left, std::make_unique<MoveLeftCommand>(&m_level.getMario()));
    
    m_inputHandler.bindKey(sf::Keyboard::Key::D, std::make_unique<MoveRightCommand>(&m_level.getMario()));
    m_inputHandler.bindKey(sf::Keyboard::Key::Right, std::make_unique<MoveRightCommand>(&m_level.getMario()));
}

void PlayState::onEnter() {
    std::cout << "Entering PlayState...\n";
}

void PlayState::onExit() {
    std::cout << "Exiting PlayState...\n";
}

void PlayState::processEvents(const sf::Event& event) {
    // Can process single-press events here if needed
    (void)event;
}

void PlayState::update(float dt) {
    // Stop Mario if no left/right keys are pressed
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) &&
        !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) &&
        !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) &&
        !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        m_level.getMario().stopMoving();
    }

    // Process continuous inputs via InputHandler
    m_inputHandler.handleInput();

    // Step physics
    PhysicsEngine::getInstance().update(dt);

    // Update level entities
    m_level.update(dt);
}

void PlayState::render(sf::RenderWindow& window) {
    m_level.render(window);
}
