/**
 * @file PauseState.cpp
 * @author TV1 (Dương)
 * @brief Pause State implementation
 */
#include "states/PauseState.h"
#include "core/GameManager.h"
#include "patterns/InputState.h"
#include "core/DisplayConfig.h"
#include <iostream>

PauseState::PauseState() : m_font(), m_text(m_font) {
    if (!m_font.openFromFile("assets/fonts/mario.ttf")) {
        std::cerr << "Failed to load font in PauseState\n";
    }
    m_text.setString("PAUSED\nPress ESC to Resume");
    m_text.setCharacterSize(20);
    
    sf::FloatRect bounds = m_text.getLocalBounds();
    m_text.setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
    m_text.setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, DisplayConfig::LOGICAL_HEIGHT / 2.f});
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

void PauseState::render(sf::RenderTarget& target) {
    // GameManager will render the previous state first, so this acts as an overlay
    target.setView(target.getDefaultView());
    
    // Draw a semi-transparent black overlay
    sf::RectangleShape overlay(sf::Vector2f(target.getSize().x, target.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    target.draw(overlay);

    target.draw(m_text);
}
