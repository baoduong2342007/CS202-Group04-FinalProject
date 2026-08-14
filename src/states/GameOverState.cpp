/**
 * @file GameOverState.cpp
 * @author TV1 (Dương)
 * @brief Game Over State implementation
 */
#include "states/GameOverState.h"
#include "states/MenuState.h"
#include "core/GameManager.h"
#include "core/SoundManager.h"
#include "patterns/InputState.h"
#include "ui/UILayoutHelper.h"
#include "states/PlayState.h"
#include <algorithm>
#include <iostream>

namespace {
    constexpr unsigned int TITLE_FONT_SIZE = 24;
    constexpr unsigned int SCORE_FONT_SIZE = 14;
    constexpr float TITLE_OFFSET_Y = 40.f;
    constexpr float SCORE_OFFSET_Y = 80.f;
    constexpr float HIGH_SCORE_OFFSET_Y = 105.f;
    constexpr float MENU_OFFSET_Y = 20.f;
    constexpr const char* FONT_PATH = "assets/fonts/mario.ttf";
}

GameOverState::GameOverState(const GameProgress& progress)
    : m_font(), m_fontLoaded(false), m_progress(progress) {
    m_fontLoaded = m_font.openFromFile(FONT_PATH);
    if (!m_fontLoaded) {
#ifdef DEBUG
        std::cerr << "[DEBUG][GameOverState] Failed to load packaged font from '" << FONT_PATH << "'. Text rendering is disabled.\n";
#endif
    } else {
        m_titleText.emplace(m_font);
        m_titleText->setString("GAME OVER");
        m_titleText->setCharacterSize(TITLE_FONT_SIZE);
        m_titleText->setFillColor(sf::Color::Red);
        UILayoutHelper::setPosition(*m_titleText, UIAnchor::TopCenter, {0.f, TITLE_OFFSET_Y});

        m_scoreText.emplace(m_font);
        m_scoreText->setString("FINAL SCORE: " + std::to_string(m_progress.score));
        m_scoreText->setCharacterSize(SCORE_FONT_SIZE);
        m_scoreText->setFillColor(sf::Color::White);
        UILayoutHelper::setPosition(*m_scoreText, UIAnchor::TopCenter, {0.f, SCORE_OFFSET_Y});

        m_highScoreText.emplace(m_font);
        const int highScore = std::max(
            m_progress.score,
            GameManager::getInstance().getSaveManager().getData().highScore);
        m_highScoreText->setString("HIGH SCORE: " + std::to_string(highScore));
        m_highScoreText->setCharacterSize(SCORE_FONT_SIZE);
        m_highScoreText->setFillColor(sf::Color::Yellow);
        UILayoutHelper::setPosition(*m_highScoreText, UIAnchor::TopCenter,
                                    {0.f, HIGH_SCORE_OFFSET_Y});

        m_menu = std::make_unique<UIMenuWidget>(m_font);
        const CharacterType retryCharacter = m_progress.character;
        m_menu->addItem("RETRY", [this, retryCharacter]() {
            if (m_transitioning) return;
            m_transitioning = true;
            GameManager::getInstance().changeState(
                std::make_unique<PlayState>(retryCharacter));
        });
        m_menu->addItem("QUIT TO MENU", [this]() {
            if (m_transitioning) return;
            m_transitioning = true;
            GameManager::getInstance().changeState(std::make_unique<MenuState>());
        });
        m_menu->setPosition(UILayoutHelper::getAnchorPosition(UIAnchor::Center) + sf::Vector2f(0.f, MENU_OFFSET_Y), UIAnchor::TopCenter);
    }
}

void GameOverState::onEnter() {
    SoundManager::getInstance().playMusic(MusicId::GAME_OVER);
    GameManager::getInstance().getSaveManager().updateHighScore(m_progress.score);
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
    if (m_menu) {
        m_menu->update(dt);
    }
}

void GameOverState::render(sf::RenderTarget& target) {
    target.clear(sf::Color::Black);
    target.setView(target.getDefaultView());
    if (m_fontLoaded) {
        if (m_titleText) target.draw(*m_titleText);
        if (m_scoreText) target.draw(*m_scoreText);
        if (m_highScoreText) target.draw(*m_highScoreText);
    }
    if (m_menu) {
        m_menu->draw(target);
    }
}
