/**
 * @file WinState.cpp
 * @author TV1 (Dương)
 * @brief Win State implementation
 */
#include "states/WinState.h"
#include "states/MenuState.h"
#include "core/GameManager.h"
#include "patterns/InputState.h"
#include "core/DisplayConfig.h"
#include <iostream>

WinState::WinState(const GameProgress& progress)
    : m_font(), m_text(m_font), m_progress(progress), m_scoreText(m_font) {
    if (!m_font.openFromFile("assets/fonts/mario.ttf")) {
        std::cerr << "Failed to load font in WinState\n";
    }
    m_text.setString("YOU WIN!\nPress ENTER to Menu");
    m_text.setCharacterSize(20);
    m_text.setFillColor(sf::Color::White);
    
    sf::FloatRect bounds = m_text.getLocalBounds();
    m_text.setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
    m_text.setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, DisplayConfig::LOGICAL_HEIGHT / 2.f - 20.f});

    m_scoreText.setString("FINAL SCORE: " + std::to_string(m_progress.score));
    m_scoreText.setCharacterSize(14);
    m_scoreText.setFillColor(sf::Color::Yellow);
    
    sf::FloatRect scoreBounds = m_scoreText.getLocalBounds();
    m_scoreText.setOrigin({scoreBounds.position.x + scoreBounds.size.x / 2.f,
                           scoreBounds.position.y + scoreBounds.size.y / 2.f});
    m_scoreText.setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, DisplayConfig::LOGICAL_HEIGHT / 2.f + 30.f});
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

void WinState::render(sf::RenderTarget& target) {
    target.clear(sf::Color::Black);
    target.setView(target.getDefaultView());
    target.draw(m_text);
    target.draw(m_scoreText);
}
