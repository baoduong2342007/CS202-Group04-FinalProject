/**
 * @file GameOverState.cpp
 * @author TV1 (Dương)
 * @brief Game Over State implementation
 */
#include "states/GameOverState.h"
#include "states/MenuState.h"
#include "core/GameManager.h"
#include <iostream>

GameOverState::GameOverState() : m_font(), m_text(m_font) {
    if (!m_font.openFromFile("assets/fonts/mario.ttf")) {
        std::cerr << "Failed to load font in GameOverState\n";
    }
    m_text.setString("GAME OVER\n\nPRESS ENTER OR CLICK TO MENU");
    m_text.setCharacterSize(32);
    m_text.setFillColor(sf::Color::White);
    
    sf::FloatRect bounds = m_text.getLocalBounds();
    m_text.setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
    m_text.setPosition({640.f, 360.f});
}

void GameOverState::onEnter() {}
void GameOverState::onExit() {}

void GameOverState::processEvents(const sf::Event& event) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Enter) {
            GameManager::getInstance().changeState(std::make_unique<MenuState>());
        }
    }
    if (const auto* mouse = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouse->button == sf::Mouse::Button::Left) {
            GameManager::getInstance().changeState(std::make_unique<MenuState>());
        }
    }
}

void GameOverState::update(float dt) { (void)dt; }

void GameOverState::render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    window.setView(window.getDefaultView());
    window.draw(m_text);
}
