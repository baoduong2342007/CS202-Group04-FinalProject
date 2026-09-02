/**
 * @file WinState.cpp
 * @author TV1 (Duong) & TV2 (Nhat)
 * @brief Polished retro arcade Victory Win State implementation
 */
#include "states/WinState.h"
#include "states/MenuState.h"
#include "core/GameManager.h"
#include "core/SoundManager.h"
#include "patterns/InputState.h"
#include "ui/UILayoutHelper.h"
#include "states/PlayState.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace {
    constexpr unsigned int TITLE_FONT_SIZE = 26;
    constexpr unsigned int SUBTITLE_FONT_SIZE = 10;
    constexpr unsigned int STAT_FONT_SIZE = 11;
    constexpr unsigned int HINT_FONT_SIZE = 8;

    constexpr float PANEL_X = 120.f;
    constexpr float PANEL_Y = 28.f;
    constexpr float PANEL_WIDTH = 400.f;
    constexpr float PANEL_HEIGHT = 304.f;

    constexpr const char* FONT_PATH = "assets/fonts/mario.ttf";

    const sf::Color PANEL_COLOR(12, 20, 48);
    const sf::Color SCORE_BOX_COLOR(8, 12, 32);
    const sf::Color GOLD_COLOR(255, 215, 0);
    const sf::Color MUTED_GOLD_COLOR(255, 231, 128);
    const sf::Color BODY_COLOR(235, 240, 255);
    const sf::Color GREEN_ACCENT(40, 220, 90);
}

WinState::WinState(const GameProgress& progress)
    : m_font(), m_fontLoaded(false), m_progress(progress) {
    m_fontLoaded = m_font.openFromFile(FONT_PATH);
    if (m_fontLoaded) {
        m_font.setSmooth(false);
    }

    // Outer Victory Panel
    m_panel.setSize({PANEL_WIDTH, PANEL_HEIGHT});
    m_panel.setPosition({PANEL_X, PANEL_Y});
    m_panel.setFillColor(PANEL_COLOR);
    m_panel.setOutlineColor(GOLD_COLOR);
    m_panel.setOutlineThickness(2.5f);

    // Inner Scorecard Box
    m_scoreBox.setSize({340.f, 86.f});
    m_scoreBox.setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f - 170.f, PANEL_Y + 74.f});
    m_scoreBox.setFillColor(SCORE_BOX_COLOR);
    m_scoreBox.setOutlineColor(sf::Color(40, 70, 140));
    m_scoreBox.setOutlineThickness(1.2f);

    if (m_fontLoaded) {
        // Title
        m_titleText.emplace(m_font, "COURSE CLEAR!", TITLE_FONT_SIZE);
        m_titleText->setFillColor(GOLD_COLOR);
        m_titleText->setOutlineColor(sf::Color::Black);
        m_titleText->setOutlineThickness(2.f);
        m_titleText->setStyle(sf::Text::Bold);
        sf::FloatRect titleB = m_titleText->getLocalBounds();
        m_titleText->setOrigin({titleB.position.x + titleB.size.x / 2.f, 0.f});
        m_titleText->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, PANEL_Y + 16.f});

        // Subtitle
        m_subtitleText.emplace(m_font, "CONGRATULATIONS HERO!", SUBTITLE_FONT_SIZE);
        m_subtitleText->setFillColor(sf::Color(120, 255, 170));
        m_subtitleText->setOutlineColor(sf::Color::Black);
        m_subtitleText->setOutlineThickness(1.f);
        sf::FloatRect subB = m_subtitleText->getLocalBounds();
        m_subtitleText->setOrigin({subB.position.x + subB.size.x / 2.f, 0.f});
        m_subtitleText->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, PANEL_Y + 48.f});

        // Score Text
        std::string scoreStr = std::to_string(m_progress.score);
        while (scoreStr.length() < 6) scoreStr = "0" + scoreStr;
        m_scoreText.emplace(m_font, "STAGE 4 SCORE : " + scoreStr, STAT_FONT_SIZE);
        m_scoreText->setFillColor(GOLD_COLOR);
        m_scoreText->setOutlineColor(sf::Color::Black);
        m_scoreText->setOutlineThickness(1.f);
        m_scoreText->setPosition({PANEL_X + 67.f, PANEL_Y + 84.f});

        // High Score Text — lookup the final stage's (World 1-4) best record.
        const int lastLevel = LevelCatalog::count();
        const int stageHighScore = GameManager::getInstance().getSaveManager().getLevelHighScore(lastLevel);
        const int highScore = std::max(m_progress.score, stageHighScore);
        std::string highStr = std::to_string(highScore);
        while (highStr.length() < 6) highStr = "0" + highStr;
        m_highScoreText.emplace(m_font, "STAGE 4 BEST  : " + highStr, STAT_FONT_SIZE);
        m_highScoreText->setFillColor(BODY_COLOR);
        m_highScoreText->setOutlineColor(sf::Color::Black);
        m_highScoreText->setOutlineThickness(1.f);
        m_highScoreText->setPosition({PANEL_X + 67.f, PANEL_Y + 108.f});

        // Level / World status
        m_levelText.emplace(m_font, "WORLD STATUS  : CLEARED!", STAT_FONT_SIZE);
        m_levelText->setFillColor(GREEN_ACCENT);
        m_levelText->setOutlineColor(sf::Color::Black);
        m_levelText->setOutlineThickness(1.f);
        m_levelText->setPosition({PANEL_X + 67.f, PANEL_Y + 132.f});

        // Hint Text
        m_hintText.emplace(m_font, "[UP / DOWN] CHOOSE    [ENTER / CLICK] SELECT", HINT_FONT_SIZE);
        m_hintText->setFillColor(sf::Color(180, 210, 250));
        m_hintText->setOutlineColor(sf::Color::Black);
        m_hintText->setOutlineThickness(1.f);
        sf::FloatRect hintB = m_hintText->getLocalBounds();
        m_hintText->setOrigin({hintB.position.x + hintB.size.x / 2.f, 0.f});
        m_hintText->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, PANEL_Y + 276.f});

        // Menu
        m_menu = std::make_unique<UIMenuWidget>(m_font);
        const int nextLevel = m_progress.currentLevel + 1;
        const CharacterType playCharacter = m_progress.character;
        const CharacterType playCharacterTwo = m_progress.character2;
        const bool isCoop = m_progress.isCoop;

        if (nextLevel <= 4) {
            m_menu->addItem("NEXT STAGE", [this, nextLevel, playCharacter, playCharacterTwo, isCoop]() {
                if (m_transitioning) return;
                m_transitioning = true;
                SoundManager::getInstance().playSound(SoundId::COIN);
                GameManager::getInstance().changeState(
                    isCoop
                        ? std::make_unique<PlayState>(nextLevel, playCharacter, playCharacterTwo)
                        : std::make_unique<PlayState>(nextLevel, playCharacter));
            }, 13);
        } else {
            m_menu->addItem("PLAY AGAIN", [this, playCharacter, playCharacterTwo, isCoop]() {
                if (m_transitioning) return;
                m_transitioning = true;
                SoundManager::getInstance().playSound(SoundId::COIN);
                GameManager::getInstance().changeState(
                    isCoop
                        ? std::make_unique<PlayState>(1, playCharacter, playCharacterTwo)
                        : std::make_unique<PlayState>(1, playCharacter));
            }, 13);
        }

        m_menu->addItem("RETURN TO MENU", [this]() {
            if (m_transitioning) return;
            m_transitioning = true;
            SoundManager::getInstance().playSound(SoundId::POWER_DOWN);
            GameManager::getInstance().changeState(std::make_unique<MenuState>());
        }, 13);

        m_menu->setSpacing(20.f);
        m_menu->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, PANEL_Y + 210.f}, UIAnchor::Center);
    }
}

void WinState::onEnter() {
    m_animTimer = 0.f;
    m_transitioning = false;
    SoundManager::getInstance().playMusic(MusicId::WIN);
    GameManager::getInstance().getSaveManager().updateHighestUnlockedLevel(LevelCatalog::count() + 1);
    const int stageScore = m_progress.score;
    GameManager::getInstance().getSaveManager().updateHighScore(stageScore, stageScore, LevelCatalog::count());
}

void WinState::onExit() {
    SoundManager::getInstance().stopMusic();
}

void WinState::processEvents(const sf::Event& event) {
    if (!m_transitioning && m_menu) {
        m_menu->processEvents(event);
    }
}

void WinState::processInput(const InputState& inputState) {
    if (!m_transitioning && m_menu) {
        m_menu->processInput(inputState);
    }
}

void WinState::update(float dt) {
    m_animTimer += dt;
    if (m_menu) {
        m_menu->update(dt);
    }

    // Golden victory pulse
    if (m_titleText) {
        const float pulse = (std::sin(m_animTimer * 6.f) + 1.f) * 0.5f;
        m_titleText->setFillColor(sf::Color(
            255,
            static_cast<std::uint8_t>(200 + 45 * pulse),
            static_cast<std::uint8_t>(50 * pulse)
        ));
    }
}

void WinState::render(sf::RenderTarget& target) {
    target.clear(sf::Color(8, 12, 30));
    target.setView(target.getDefaultView());

    target.draw(m_panel);
    target.draw(m_scoreBox);

    if (m_fontLoaded) {
        if (m_titleText) target.draw(*m_titleText);
        if (m_subtitleText) target.draw(*m_subtitleText);
        if (m_scoreText) target.draw(*m_scoreText);
        if (m_highScoreText) target.draw(*m_highScoreText);
        if (m_levelText) target.draw(*m_levelText);
        if (m_hintText) target.draw(*m_hintText);
    }
    if (m_menu) {
        m_menu->draw(target);
    }
}
