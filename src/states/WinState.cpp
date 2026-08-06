/**
 * @file WinState.cpp
 * @author TV1 (Dương)
 * @brief Win State implementation
 */
#include "states/WinState.h"
#include "states/MenuState.h"
#include "core/GameManager.h"
#include "patterns/InputState.h"
#include <iostream>

WinState::WinState() : m_font(), m_text(m_font) {
    if (!m_font.openFromFile("assets/fonts/mario.ttf")) {
        std::cerr << "Failed to load font in WinState\n";
    }
    m_text.setString("YOU WIN!\nPress ENTER to Menu");
    m_text.setCharacterSize(40);
    m_text.setPosition({400.f, 300.f});
}

void WinState::onEnter() {}
void WinState::onExit() {}

void WinState::processEvents(const sf::Event& event) {
    (void)event;
}

void WinState::processInput(const InputState& inputState) {
    if (inputState.wasPressed(sf::Keyboard::Key::Enter)) {
        GameManager::getInstance().changeState(std::make_unique<MenuState>());
    }
}

void WinState::update(float dt) { (void)dt; }

void WinState::render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    window.setView(window.getDefaultView());
    window.draw(m_text);
}
