/**
 * @file GameOverState.cpp
 * @author TV1 (Dương)
 * @brief Game Over State implementation
 */
#include "states/GameOverState.h"
#include "states/MenuState.h"
#include "core/GameManager.h"
#include "patterns/InputState.h"
#include <iostream>

GameOverState::GameOverState(const GameProgress& progress)
    : m_font(), m_text(m_font), m_progress(progress), m_scoreText(m_font) {
    if (!m_font.openFromFile("assets/fonts/mario.ttf")) {
        std::cerr << "Failed to load font in GameOverState\n";
    }
    m_text.setString("GAME OVER\n\nPRESS ENTER OR CLICK TO MENU");
    m_text.setCharacterSize(32);
    m_text.setFillColor(sf::Color::White);
    
    sf::FloatRect bounds = m_text.getLocalBounds();
    m_text.setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
    m_text.setPosition({640.f, 360.f});

    m_scoreText.setString("SCORE: " + std::to_string(m_progress.score));
    m_scoreText.setCharacterSize(24);
    m_scoreText.setFillColor(sf::Color::Yellow);
    sf::FloatRect scoreBounds = m_scoreText.getLocalBounds();
    m_scoreText.setOrigin({scoreBounds.position.x + scoreBounds.size.x / 2.f,
                           scoreBounds.position.y + scoreBounds.size.y / 2.f});
    m_scoreText.setPosition({640.f, 420.f});
}

void GameOverState::onEnter() {}
void GameOverState::onExit() {}

void GameOverState::processEvents(const sf::Event& event) {
    if (const auto* mouse = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouse->button == sf::Mouse::Button::Left) {
            GameManager::getInstance().changeState(std::make_unique<MenuState>());
        }
    }
}

void GameOverState::processInput(const InputState& inputState) {
    if (inputState.wasPressed(sf::Keyboard::Key::Enter)) {
        GameManager::getInstance().changeState(std::make_unique<MenuState>());
    }
}

void GameOverState::update(float dt) { (void)dt; }

void GameOverState::render(sf::RenderTarget& target) {
    target.clear(sf::Color::Black);
    target.setView(target.getDefaultView());
    target.draw(m_text);
    target.draw(m_scoreText);
}
