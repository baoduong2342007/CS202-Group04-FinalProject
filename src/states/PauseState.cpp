/**
 * @file PauseState.cpp
 * @author TV1 (Dương)
 * @brief Pause State implementation
 */
#include "states/PauseState.h"
#include "core/GameManager.h"
#include "patterns/InputState.h"
#include "core/DisplayConfig.h"
#include "ui/UILayoutHelper.h"
#include "states/MenuState.h"
#include <iostream>

PauseState::PauseState() : m_font(), m_titleText(m_font) {
    if (!m_font.openFromFile("assets/fonts/mario.ttf")) {
        std::cerr << "Failed to load font in PauseState\n";
    }
    
    m_titleText.setString("PAUSED");
    m_titleText.setCharacterSize(24);
    m_titleText.setFillColor(sf::Color::White);
    UILayoutHelper::setPosition(m_titleText, UIAnchor::Center, {0.f, -40.f});

    m_menu = std::make_unique<UIMenuWidget>(m_font);
    m_menu->addItem("RESUME", []() {
        GameManager::getInstance().popState();
    });
    m_menu->addItem("QUIT TO MENU", []() {
        GameManager::getInstance().changeState(std::make_unique<MenuState>());
    });
    
    // Position menu in the center, slightly below the title
    m_menu->setPosition(UILayoutHelper::getAnchorPosition(UIAnchor::Center) + sf::Vector2f(0.f, 10.f), UIAnchor::TopCenter);
}

void PauseState::onEnter() {}
void PauseState::onExit() {}

void PauseState::processEvents(const sf::Event& event) {
    if (m_menu) {
        m_menu->processEvents(event);
    }
}

void PauseState::processInput(const InputState& inputState) {
    if (inputState.wasPressed(sf::Keyboard::Key::Escape)) {
        GameManager::getInstance().popState();
    }
    
    if (m_menu) {
        m_menu->processInput(inputState);
    }
}

void PauseState::update(float dt) {
    if (m_menu) {
        m_menu->update(dt);
    }
}

void PauseState::render(sf::RenderTarget& target) {
    // GameManager will render the previous state first, so this acts as an overlay
    target.setView(target.getDefaultView());
    
    // Draw a semi-transparent black overlay
    sf::RectangleShape overlay(sf::Vector2f(target.getSize().x, target.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    target.draw(overlay);

    target.draw(m_titleText);
    
    if (m_menu) {
        m_menu->draw(target);
    }
}
