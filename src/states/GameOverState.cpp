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
    m_text.setString("GAME OVER\nPress ENTER to Menu");
    m_text.setCharacterSize(40);
    m_text.setPosition({400.f, 300.f});
}

void GameOverState::onEnter() {}
void GameOverState::onExit() {}

void GameOverState::processEvents(const sf::Event& event) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Enter) {
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
