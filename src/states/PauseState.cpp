/**
 * @file PauseState.cpp
 * @author TV1 (Dương)
 * @brief Pause State implementation
 */
#include "states/PauseState.h"
#include "core/GameManager.h"
#include "patterns/InputState.h"
#include <iostream>

PauseState::PauseState() : m_font(), m_text(m_font) {
    if (!m_font.openFromFile("assets/fonts/mario.ttf")) {
        std::cerr << "Failed to load font in PauseState\n";
    }
    m_text.setString("PAUSED\nPress ESC to Resume");
    m_text.setCharacterSize(40);
    m_text.setPosition({400.f, 300.f});
}

void PauseState::onEnter() {}
void PauseState::onExit() {}

void PauseState::processEvents(const sf::Event& event) {
    (void)event;
}

void PauseState::processInput(const InputState& inputState) {
    if (inputState.wasPressed(sf::Keyboard::Key::Escape)) {
        GameManager::getInstance().popState();
    }
}

void PauseState::update(float dt) { (void)dt; }

void PauseState::render(sf::RenderWindow& window) {
    // GameManager will render the previous state first, so this acts as an overlay
    window.setView(window.getDefaultView());
    
    // Draw a semi-transparent black overlay
    sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    window.draw(m_text);
}
