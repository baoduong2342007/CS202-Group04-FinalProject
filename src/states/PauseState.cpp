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

namespace {
    constexpr unsigned int TITLE_FONT_SIZE = 24;
    constexpr unsigned int VOLUME_FONT_SIZE = 12;
    constexpr float TITLE_OFFSET_Y = 30.f;
    constexpr float MENU_OFFSET_Y = 30.f;
    constexpr float VOLUME_OFFSET_Y = -10.f;
    constexpr std::uint8_t OVERLAY_ALPHA = 150;
    constexpr const char* FONT_PATH = "assets/fonts/mario.ttf";
}

PauseState::PauseState() : m_font(), m_fontLoaded(false) {
    m_fontLoaded = m_font.openFromFile(FONT_PATH);
    if (!m_fontLoaded) {
#ifdef DEBUG
        std::cerr << "[DEBUG][PauseState] Failed to load packaged font from '" << FONT_PATH << "'. Text rendering is disabled.\n";
#endif
    } else {
        m_titleText.emplace(m_font);
        m_titleText->setString("PAUSED");
        m_titleText->setCharacterSize(TITLE_FONT_SIZE);
        m_titleText->setFillColor(sf::Color::White);
        UILayoutHelper::setPosition(*m_titleText, UIAnchor::TopCenter, {0.f, TITLE_OFFSET_Y});

        m_volumeText.emplace(m_font);
        m_volumeText->setCharacterSize(VOLUME_FONT_SIZE);
        m_volumeText->setFillColor(sf::Color::Yellow);
        refreshText();

        m_menu = std::make_unique<UIMenuWidget>(m_font);
        m_menu->addItem("RESUME", []() {
            GameManager::getInstance().popState();
        });
        m_menu->addItem("QUIT TO MENU", []() {
            GameManager::getInstance().changeState(std::make_unique<MenuState>());
        });

        m_menu->setPosition(UILayoutHelper::getAnchorPosition(UIAnchor::Center) + sf::Vector2f(0.f, MENU_OFFSET_Y), UIAnchor::TopCenter);
    }
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
    overlay.setFillColor(sf::Color(0, 0, 0, OVERLAY_ALPHA));
    target.draw(overlay);

    if (m_fontLoaded) {
        if (m_titleText) target.draw(*m_titleText);
        if (m_volumeText) target.draw(*m_volumeText);
    }

    if (m_menu) {
        m_menu->draw(target);
    }
}

void PauseState::refreshText() {
    if (!m_fontLoaded || !m_volumeText) return;

    const SoundManager& sound = SoundManager::getInstance();
    std::ostringstream text;
    text << (m_selectedVolume == PauseVolumeSelection::MUSIC ? "> " : "  ")
         << "MUSIC: " << std::fixed << std::setprecision(0)
         << sound.getMusicVolume() << "%  |  "
         << (m_selectedVolume == PauseVolumeSelection::SOUND ? "> " : "  ")
         << "SFX: " << std::fixed << std::setprecision(0)
         << sound.getSoundVolume() << "%\n"
         << "  [LEFT/RIGHT ADJUST VOLUME]";
    m_volumeText->setString(text.str());
    UILayoutHelper::setPosition(*m_volumeText, UIAnchor::Center, {0.f, VOLUME_OFFSET_Y});
}

void PauseState::adjustSelectedVolume(float delta) {
    SoundManager& sound = SoundManager::getInstance();
    if (m_selectedVolume == PauseVolumeSelection::MUSIC) {
        sound.setMusicVolume(sound.getMusicVolume() + delta);
    } else {
        sound.setSoundVolume(sound.getSoundVolume() + delta);
    }

    // SaveManager is the persistence owner. Persist both values after every
    // adjustment so a pause-menu change survives an application restart.
    GameManager::getInstance().getSaveManager().updateAudioSettings(
        sound.getSoundVolume(), sound.getMusicVolume());
    refreshText();
}
