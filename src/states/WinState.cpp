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
#include "ui/UILayoutHelper.h"
#include <iostream>

WinState::WinState(const GameProgress& progress)
    : m_font(), m_titleText(m_font), m_progress(progress), m_scoreText(m_font) {
    if (!m_font.openFromFile("assets/fonts/mario.ttf")) {
        std::cerr << "Failed to load font in WinState\n";
    }
    
    m_titleText.setString("YOU WIN!");
    m_titleText.setCharacterSize(24);
    m_titleText.setFillColor(sf::Color::White);
    UILayoutHelper::setPosition(m_titleText, UIAnchor::TopCenter, {0.f, 40.f});

    m_scoreText.setString("FINAL SCORE: " + std::to_string(m_progress.score));
    m_scoreText.setCharacterSize(14);
    m_scoreText.setFillColor(sf::Color::Yellow);
    UILayoutHelper::setPosition(m_scoreText, UIAnchor::TopCenter, {0.f, 80.f});

    m_menu = std::make_unique<UIMenuWidget>(m_font);
    
    m_menu->addItem("RETURN TO MENU", []() {
        GameManager::getInstance().changeState(std::make_unique<MenuState>());
    });
    
    m_menu->setPosition(UILayoutHelper::getAnchorPosition(UIAnchor::Center) + sf::Vector2f(0.f, 20.f), UIAnchor::TopCenter);
}

void WinState::onEnter() {}
void WinState::onExit() {}

void WinState::processEvents(const sf::Event& event) {
    if (m_menu) {
        m_menu->processEvents(event);
    }
}

void WinState::processInput(const InputState& inputState) {
    if (m_menu) {
        m_menu->processInput(inputState);
    }
}

void WinState::update(float dt) {
    if (m_menu) {
        m_menu->update(dt);
    }
}

void WinState::render(sf::RenderTarget& target) {
    target.clear(sf::Color::Black);
    target.setView(target.getDefaultView());
    target.draw(m_titleText);
    target.draw(m_scoreText);
    if (m_menu) {
        m_menu->draw(target);
    }
}
