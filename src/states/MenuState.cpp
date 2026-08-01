/**
 * @file MenuState.cpp
 * @author TV1 (Dương)
 * @brief Main Menu State implementation
 */
#include "states/MenuState.h"
#include "states/PlayState.h"
#include "core/GameManager.h"
#include <iostream>

MenuState::MenuState() : m_font(), m_text(m_font) {
    if (!m_font.openFromFile("assets/fonts/mario.ttf")) {
        std::cerr << "Failed to load font in MenuState\n";
    }
    m_text.setString("SUPER MARIO\nPress ENTER to Play");
    m_text.setCharacterSize(40);
    m_text.setPosition({400.f, 300.f});
}

void MenuState::onEnter() {}
void MenuState::onExit() {}

void MenuState::processEvents(const sf::Event& event) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Enter) {
            GameManager::getInstance().changeState(std::make_unique<PlayState>());
        }
    }
}

void MenuState::update(float dt) { (void)dt; }

void MenuState::render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    window.setView(window.getDefaultView());
    window.draw(m_text);
}
