/**
 * @file GameOverState.cpp
 * @author TV1 (Dương)
 * @brief Game Over State implementation
 */
#include "states/GameOverState.h"
#include "states/MenuState.h"
#include "core/GameManager.h"
#include "patterns/InputState.h"
#include "core/DisplayConfig.h"
#include "ui/UILayoutHelper.h"
#include "states/PlayState.h"
#include <iostream>

GameOverState::GameOverState(const GameProgress& progress)
    : m_font(), m_titleText(m_font), m_progress(progress), m_scoreText(m_font) {
    if (!m_font.openFromFile("assets/fonts/mario.ttf")) {
        std::cerr << "Failed to load font in GameOverState\n";
    }
    
    m_titleText.setString("GAME OVER");
    m_titleText.setCharacterSize(24);
    m_titleText.setFillColor(sf::Color::Red);
    UILayoutHelper::setPosition(m_titleText, UIAnchor::TopCenter, {0.f, 40.f});

    m_scoreText.setString("FINAL SCORE: " + std::to_string(m_progress.score));
    m_scoreText.setCharacterSize(14);
    m_scoreText.setFillColor(sf::Color::White);
    UILayoutHelper::setPosition(m_scoreText, UIAnchor::TopCenter, {0.f, 80.f});

    m_menu = std::make_unique<UIMenuWidget>(m_font);
    
    m_menu->addItem("RETRY", [this]() {
        // Reset progress but keep the level (assuming progress carries level index if supported, otherwise just start over)
        GameProgress newProgress;
        GameManager::getInstance().changeState(std::make_unique<PlayState>());
    });
    
    m_menu->addItem("QUIT TO MENU", []() {
        GameManager::getInstance().changeState(std::make_unique<MenuState>());
    });
    
    m_menu->setPosition(UILayoutHelper::getAnchorPosition(UIAnchor::Center) + sf::Vector2f(0.f, 20.f), UIAnchor::TopCenter);
}

void GameOverState::onEnter() {}
void GameOverState::onExit() {}

void GameOverState::processEvents(const sf::Event& event) {
    if (m_menu) {
        m_menu->processEvents(event);
    }
}

void GameOverState::processInput(const InputState& inputState) {
    if (m_menu) {
        m_menu->processInput(inputState);
    }
}

void GameOverState::update(float dt) {
    if (m_menu) {
        m_menu->update(dt);
    }
}

void GameOverState::render(sf::RenderTarget& target) {
    target.clear(sf::Color::Black);
    target.setView(target.getDefaultView());
    target.draw(m_titleText);
    target.draw(m_scoreText);
    if (m_menu) {
        m_menu->draw(target);
    }
}
