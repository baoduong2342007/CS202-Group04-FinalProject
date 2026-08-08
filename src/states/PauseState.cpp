/**
 * @file PauseState.cpp
 * @author TV1 (Dương)
 * @brief Pause State implementation
 */
#include "states/PauseState.h"
#include "core/GameManager.h"
#include "core/SoundManager.h"
#include "patterns/InputState.h"
#include <iomanip>
#include <iostream>
#include <sstream>

PauseState::PauseState() : m_font(), m_text(m_font) {
    if (!m_font.openFromFile("assets/fonts/mario.ttf")) {
        std::cerr << "Failed to load font in PauseState\n";
    }
    m_text.setCharacterSize(40);
    m_text.setPosition({400.f, 300.f});
    refreshText();
}

void PauseState::onEnter() {
    refreshText();
}
void PauseState::onExit() {}

void PauseState::processEvents(const sf::Event& event) {
    (void)event;
}

void PauseState::processInput(const InputState& inputState) {
    if (inputState.wasPressed(sf::Keyboard::Key::Escape)) {
        GameManager::getInstance().popState();
        return;
    }

    if (inputState.wasPressed(sf::Keyboard::Key::Up) ||
        inputState.wasPressed(sf::Keyboard::Key::Down)) {
        m_selectedVolume = m_selectedVolume == PauseVolumeSelection::MUSIC
                               ? PauseVolumeSelection::SOUND
                               : PauseVolumeSelection::MUSIC;
        refreshText();
        return;
    }

    if (inputState.wasPressed(sf::Keyboard::Key::M)) {
        m_selectedVolume = PauseVolumeSelection::MUSIC;
        refreshText();
        return;
    }

    if (inputState.wasPressed(sf::Keyboard::Key::S)) {
        m_selectedVolume = PauseVolumeSelection::SOUND;
        refreshText();
        return;
    }

    if (inputState.wasPressed(sf::Keyboard::Key::Left)) {
        adjustSelectedVolume(-5.f);
        return;
    }

    if (inputState.wasPressed(sf::Keyboard::Key::Right)) {
        adjustSelectedVolume(5.f);
    }
}

void PauseState::update(float dt) { (void)dt; }

void PauseState::render(sf::RenderTarget& target) {
    // GameManager will render the previous state first, so this acts as an overlay
    target.setView(target.getDefaultView());
    
    // Draw a semi-transparent black overlay
    sf::RectangleShape overlay(sf::Vector2f(target.getSize().x, target.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    target.draw(overlay);

    target.draw(m_text);
}

void PauseState::refreshText() {
    const SoundManager& sound = SoundManager::getInstance();
    std::ostringstream text;
    text << "PAUSED\n\n"
         << (m_selectedVolume == PauseVolumeSelection::MUSIC ? "> " : "  ")
         << "MUSIC " << std::fixed << std::setprecision(0)
         << sound.getMusicVolume() << "%\n"
         << (m_selectedVolume == PauseVolumeSelection::SOUND ? "> " : "  ")
         << "SFX   " << std::fixed << std::setprecision(0)
         << sound.getSoundVolume() << "%\n\n"
         << "UP/DOWN SELECT  LEFT/RIGHT ADJUST\n"
         << "ESC RESUME";
    m_text.setString(text.str());
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
