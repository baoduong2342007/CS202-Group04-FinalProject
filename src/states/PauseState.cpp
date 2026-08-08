/**
 * @file PauseState.cpp
 * @author TV1 (Dương) & TV2 (Nhật)
 * @brief Pause State implementation
 */
#include "states/PauseState.h"
#include "core/GameManager.h"
#include "core/SoundManager.h"
#include "patterns/InputState.h"
#include "ui/UILayoutHelper.h"
#include "states/MenuState.h"
#include <iomanip>
#include <iostream>
#include <sstream>

PauseState::PauseState() : m_font(), m_titleText(m_font), m_volumeText(m_font) {
    if (!m_font.openFromFile("assets/fonts/mario.ttf")) {
        std::cerr << "Failed to load font in PauseState\n";
    }

    m_titleText.setString("PAUSED");
    m_titleText.setCharacterSize(24);
    m_titleText.setFillColor(sf::Color::White);
    UILayoutHelper::setPosition(m_titleText, UIAnchor::TopCenter, {0.f, 30.f});

    m_volumeText.setCharacterSize(12);
    m_volumeText.setFillColor(sf::Color::Yellow);
    refreshText();

    m_menu = std::make_unique<UIMenuWidget>(m_font);
    m_menu->addItem("RESUME", []() {
        GameManager::getInstance().popState();
    });
    m_menu->addItem("QUIT TO MENU", []() {
        GameManager::getInstance().changeState(std::make_unique<MenuState>());
    });

    m_menu->setPosition(UILayoutHelper::getAnchorPosition(UIAnchor::Center) + sf::Vector2f(0.f, 30.f), UIAnchor::TopCenter);
}

void PauseState::onEnter() {
    refreshText();
}

void PauseState::onExit() {}

void PauseState::processEvents(const sf::Event& event) {
    if (m_menu) {
        m_menu->processEvents(event);
    }
}

void PauseState::processInput(const InputState& inputState) {
    if (inputState.wasPressed(sf::Keyboard::Key::Escape)) {
        GameManager::getInstance().popState();
        return;
    }

    if (inputState.wasPressed(sf::Keyboard::Key::M)) {
        m_selectedVolume = PauseVolumeSelection::MUSIC;
        refreshText();
    } else if (inputState.wasPressed(sf::Keyboard::Key::S)) {
        m_selectedVolume = PauseVolumeSelection::SOUND;
        refreshText();
    }

    if (inputState.wasPressed(sf::Keyboard::Key::Left)) {
        adjustSelectedVolume(-5.f);
    } else if (inputState.wasPressed(sf::Keyboard::Key::Right)) {
        adjustSelectedVolume(5.f);
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
    target.setView(target.getDefaultView());

    sf::RectangleShape overlay(sf::Vector2f(target.getSize().x, target.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    target.draw(overlay);

    target.draw(m_titleText);
    target.draw(m_volumeText);

    if (m_menu) {
        m_menu->draw(target);
    }
}

void PauseState::refreshText() {
    const SoundManager& sound = SoundManager::getInstance();
    std::ostringstream text;
    text << (m_selectedVolume == PauseVolumeSelection::MUSIC ? "> " : "  ")
         << "MUSIC: " << std::fixed << std::setprecision(0)
         << sound.getMusicVolume() << "%  |  "
         << (m_selectedVolume == PauseVolumeSelection::SOUND ? "> " : "  ")
         << "SFX: " << std::fixed << std::setprecision(0)
         << sound.getSoundVolume() << "%\n"
         << "  [LEFT/RIGHT ADJUST VOLUME]";
    m_volumeText.setString(text.str());
    UILayoutHelper::setPosition(m_volumeText, UIAnchor::Center, {0.f, -10.f});
}

void PauseState::adjustSelectedVolume(float delta) {
    SoundManager& sound = SoundManager::getInstance();
    if (m_selectedVolume == PauseVolumeSelection::MUSIC) {
        sound.setMusicVolume(sound.getMusicVolume() + delta);
    } else {
        sound.setSoundVolume(sound.getSoundVolume() + delta);
    }
    refreshText();
}
