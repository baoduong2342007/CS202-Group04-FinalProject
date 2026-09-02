/**
 * @file GameOverState.cpp
 * @author TV1 (Duong)
 * @brief Polished retro arcade Game Over State implementation
 */
#include "states/GameOverState.h"
#include "states/MenuState.h"
#include "core/GameManager.h"
#include "core/SoundManager.h"
#include "core/TimeUtils.h"
#include "patterns/InputState.h"
#include "ui/UILayoutHelper.h"
#include "states/PlayState.h"
#include <algorithm>
#include <cmath>
#include <iostream>

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

    const sf::Color PANEL_COLOR(16, 12, 28);
    const sf::Color SCORE_BOX_COLOR(8, 6, 16);
    const sf::Color RED_ACCENT(220, 35, 35);
    const sf::Color GOLD_COLOR(255, 215, 0);
    const sf::Color MUTED_GOLD_COLOR(255, 231, 128);
    const sf::Color BODY_COLOR(235, 240, 255);
}

GameOverState::GameOverState(const GameProgress& progress)
    : m_font(), m_fontLoaded(false), m_progress(progress) {
    m_fontLoaded = m_font.openFromFile(FONT_PATH);
    if (m_fontLoaded) {
        m_font.setSmooth(false);
    }

    // Outer Panel
    m_panel.setSize({PANEL_WIDTH, PANEL_HEIGHT});
    m_panel.setPosition({PANEL_X, PANEL_Y});
    m_panel.setFillColor(PANEL_COLOR);
    m_panel.setOutlineColor(RED_ACCENT);
    m_panel.setOutlineThickness(2.5f);

    // Inner Scorecard Box
    m_scoreBox.setSize({340.f, 86.f});
    m_scoreBox.setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f - 170.f, PANEL_Y + 74.f});
    m_scoreBox.setFillColor(SCORE_BOX_COLOR);
    m_scoreBox.setOutlineColor(sf::Color(70, 30, 40));
    m_scoreBox.setOutlineThickness(1.2f);

    if (m_fontLoaded) {
        // Title
        m_titleText.emplace(m_font, "GAME OVER", TITLE_FONT_SIZE);
        m_titleText->setFillColor(RED_ACCENT);
        m_titleText->setOutlineColor(sf::Color::Black);
        m_titleText->setOutlineThickness(2.f);
        m_titleText->setStyle(sf::Text::Bold);
        sf::FloatRect titleB = m_titleText->getLocalBounds();
        m_titleText->setOrigin({titleB.position.x + titleB.size.x / 2.f, 0.f});
        m_titleText->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, PANEL_Y + 16.f});

        // Subtitle
        m_subtitleText.emplace(m_font, "BETTER LUCK NEXT TIME!", SUBTITLE_FONT_SIZE);
        m_subtitleText->setFillColor(sf::Color(255, 160, 140));
        m_subtitleText->setOutlineColor(sf::Color::Black);
        m_subtitleText->setOutlineThickness(1.f);
        sf::FloatRect subB = m_subtitleText->getLocalBounds();
        m_subtitleText->setOrigin({subB.position.x + subB.size.x / 2.f, 0.f});
        m_subtitleText->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, PANEL_Y + 48.f});

        // Score Text
        std::string scoreStr = std::to_string(m_progress.score);
        while (scoreStr.length() < 6) scoreStr = "0" + scoreStr;
        m_scoreText.emplace(m_font, "FINAL SCORE   : " + scoreStr, STAT_FONT_SIZE);
        m_scoreText->setFillColor(BODY_COLOR);
        m_scoreText->setOutlineColor(sf::Color::Black);
        m_scoreText->setOutlineThickness(1.f);
        m_scoreText->setPosition({PANEL_X + 46.f, PANEL_Y + 84.f});

        // High Score Text
        const int stageHighScore = GameManager::getInstance().getSaveManager().getHighScore(m_progress.currentLevel);
        const int highScore = std::max(m_progress.score, stageHighScore);
        std::string highStr = std::to_string(highScore);
        while (highStr.length() < 6) highStr = "0" + highStr;
        m_highScoreText.emplace(m_font, "STAGE BEST    : " + highStr, STAT_FONT_SIZE);
        m_highScoreText->setFillColor(GOLD_COLOR);
        m_highScoreText->setOutlineColor(sf::Color::Black);
        m_highScoreText->setOutlineThickness(1.f);
        m_highScoreText->setPosition({PANEL_X + 46.f, PANEL_Y + 108.f});

        // Level Text
        m_levelText.emplace(m_font, "STAGE REACHED : WORLD 1-" + std::to_string(m_progress.currentLevel), STAT_FONT_SIZE);
        m_levelText->setFillColor(sf::Color(130, 200, 255));
        m_levelText->setOutlineColor(sf::Color::Black);
        m_levelText->setOutlineThickness(1.f);
        m_levelText->setPosition({PANEL_X + 46.f, PANEL_Y + 132.f});

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
        const int retryLevel = m_progress.currentLevel;
        const CharacterType retryCharacter = m_progress.character;
        const CharacterType retryCharacterTwo = m_progress.character2;
        const bool retryCoop = m_progress.isCoop;

        m_menu->addItem("RETRY STAGE", [this, retryLevel, retryCharacter,
                                        retryCharacterTwo, retryCoop]() {
            if (m_transitioning) return;
            m_transitioning = true;
            SoundManager::getInstance().playSound(SoundId::COIN);
            GameManager::getInstance().changeState(
                retryCoop
                    ? std::make_unique<PlayState>(retryLevel, retryCharacter, retryCharacterTwo)
                    : std::make_unique<PlayState>(retryLevel, retryCharacter));
        }, 13);

        m_menu->addItem("QUIT TO MAIN MENU", [this]() {
            if (m_transitioning) return;
            m_transitioning = true;
            SoundManager::getInstance().playSound(SoundId::POWER_DOWN);
            GameManager::getInstance().changeState(std::make_unique<MenuState>());
        }, 13);

        m_menu->setSpacing(20.f);
        m_menu->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, PANEL_Y + 210.f}, UIAnchor::Center);
    }
}

void GameOverState::onEnter() {
    m_animTimer = 0.f;
    SoundManager::getInstance().playMusic(MusicId::GAME_OVER);
    const int stageScore = m_progress.score - m_progress.levelStartScore;
    GameManager::getInstance().getSaveManager().updateHighScore(m_progress.score, stageScore, m_progress.currentLevel);

    GameRecord rec;
    rec.date = TimeUtils::getCurrentDateTimeString();
    rec.level = m_progress.currentLevel;
    rec.mode = m_progress.isCoop ? "CO-OP" : "SOLO";
    if (m_progress.isCoop) {
        rec.character = "CO-OP";
    } else if (m_progress.character == CharacterType::LUIGI) {
        rec.character = "LUIGI";
    } else {
        rec.character = "MARIO";
    }
    rec.result = "GAME OVER";
    rec.score = stageScore;
    rec.coins = m_progress.coins - m_progress.levelStartCoins;
    GameManager::getInstance().getSaveManager().addGameRecord(rec);
}

void GameOverState::onExit() {
    SoundManager::getInstance().stopMusic();
}

void GameOverState::processEvents(const sf::Event& event) {
    if (!m_transitioning && m_menu) {
        m_menu->processEvents(event);
    }
}

void GameOverState::processInput(const InputState& inputState) {
    if (!m_transitioning && m_menu) {
        m_menu->processInput(inputState);
    }
}

void GameOverState::update(float dt) {
    m_animTimer += dt;
    if (m_menu) {
        m_menu->update(dt);
    }

    // Subtle red pulse on title
    if (m_titleText) {
        const float pulse = (std::sin(m_animTimer * 5.f) + 1.f) * 0.5f;
        m_titleText->setFillColor(sf::Color(
            static_cast<std::uint8_t>(200 + 55 * pulse),
            static_cast<std::uint8_t>(20 * (1.f - pulse)),
            static_cast<std::uint8_t>(20 * (1.f - pulse))
        ));
    }
}

void GameOverState::render(sf::RenderTarget& target) {
    target.clear(sf::Color(8, 4, 12));
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
