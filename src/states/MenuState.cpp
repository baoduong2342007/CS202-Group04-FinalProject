/**
 * @file MenuState.cpp
 * @author TV2 (Nhật)
 * @brief Sprite-based main menu with bitmap text from HUD spritesheet
 * @note Sprint 5 — replaces text-only MenuState with retro NES-style menu
 */

#include "states/MenuState.h"
#include "states/CharacterSelectState.h"
#include "core/GameManager.h"
#include "patterns/InputState.h"
#include "core/DisplayConfig.h"
#include "ui/UILayoutHelper.h"

#include <iomanip>
#include <sstream>

#ifdef DEBUG
#include <iostream>
#endif

namespace {
    const sf::IntRect BG_RECT({0, 192}, {256, 224}); // Nestopia scheme (1st column)
    const sf::IntRect CURSOR_RECT({0, 440}, {8, 8}); // Estimated mushroom coords in hud.png

    const float SCORE_POS_X = 16.f;
    const float SCORE_POS_Y = 24.f;
    const float COIN_TEXT_POS_X = 104.f;
    const float COIN_TEXT_POS_Y = 24.f;
    const float WORLD_POS_X = 152.f;
    const float WORLD_POS_Y = 24.f;
    const float TOP_SCORE_POS_X = 128.f;
    const float TOP_SCORE_POS_Y = 184.f;
    
    const float COIN_ICON_POS_X = 88.f;
    const float COIN_ICON_POS_Y = 24.f;

    const float COIN_ANIM_DURATION = 0.30f;
    const int COIN_FRAMES = 3;
    const int COIN_FRAME_SPACING = 10;
    const int COIN_BASE_X = 0;
    const int COIN_BASE_Y = 156;
    
    const unsigned int PRESS_TO_PLAY_FONT_SIZE = 14;
    const sf::Color GOLD_COLOR(255, 215, 0);
    const float OUTLINE_THICKNESS = 1.f;
    const float PRESS_TO_PLAY_OFFSET_Y = 20.f;
    const char* FONT_PATH = "assets/fonts/mario.ttf";

    sf::IntRect getCharRect(char c) {
        // Tilemap "0123456789ABCDEF" starts at X=328, Y=0
        int baseX = 0;
        int baseY = 105;
        
        if (c >= '0' && c <= '9') {
            return sf::IntRect({baseX + (c - '0') * 9, baseY}, {8, 8});
        } else if (c == '-') {
            return sf::IntRect({108, 114}, {8, 8}); // Guessing '-'
        }
        return sf::IntRect({baseX, baseY}, {8, 8});
    }
}

MenuState::MenuState(int score, int coins, int world, int level, int topScore) 
    : m_bgSprite(m_hudTexture), m_cursorSprite(m_hudTexture),
      m_font(), m_fontLoaded(false),
      m_score(score), m_coins(coins), m_world(world), m_level(level), m_topScore(topScore),
      m_coinSprite(m_hudTexture) {}

void MenuState::onEnter() {
    m_topScore = GameManager::getInstance().getSaveManager().getData().highScore;
    m_dynamicTextSprites.clear();
    m_transitioning = false;

    try {
        m_hudTexture = sf::Texture("assets/textures/ui/hud.png");
    } catch (const std::exception& e) {
#ifdef DEBUG
        std::cerr << "[DEBUG][MenuState] Failed to load hud.png: " << e.what() << "\n";
#endif
        return; // Cannot render menu without texture
    }
    
    // Scale to fit logical height
    const float SCALE = static_cast<float>(DisplayConfig::LOGICAL_HEIGHT) / 224.f;
    
    // Background
    m_bgSprite.setTexture(m_hudTexture);
    m_bgSprite.setTextureRect(BG_RECT);
    m_bgSprite.setScale({SCALE, SCALE});
    m_bgSprite.setPosition({
        (DisplayConfig::LOGICAL_WIDTH - 256.f * SCALE) / 2.f,
        (DisplayConfig::LOGICAL_HEIGHT - 224.f * SCALE) / 2.f
    });

    // Cursor (shifted 1 tile right)
    m_cursorSprite.setTexture(m_hudTexture);
    m_cursorSprite.setTextureRect(CURSOR_RECT);
    m_cursorSprite.setScale({SCALE, SCALE});
    m_cursorSprite.setPosition(m_bgSprite.getPosition() + sf::Vector2f(72.f * SCALE, 144.f * SCALE));

    // Bitmap Text (shifted 1 tile right)
    auto addText = [&](const std::string& str, float startX, float startY) {
        for (size_t i = 0; i < str.length(); ++i) {
            sf::Sprite s(m_hudTexture, getCharRect(str[i]));
            s.setScale({SCALE, SCALE});
            s.setPosition(m_bgSprite.getPosition() + sf::Vector2f((startX + i * 8.f) * SCALE, startY * SCALE));
            m_dynamicTextSprites.push_back(s);
        }
    };

    std::ostringstream ssScore;
    ssScore << std::setfill('0') << std::setw(7) << m_score;
    
    std::ostringstream ssCoins;
    ssCoins << std::setfill('0') << std::setw(2) << m_coins;
    
    std::ostringstream ssWorld;
    ssWorld << m_world << "-" << m_level;

    std::ostringstream ssTop;
    ssTop << std::setfill('0') << std::setw(7) << m_topScore;

    addText(ssScore.str(), SCORE_POS_X, SCORE_POS_Y);  
    addText(ssCoins.str(), COIN_TEXT_POS_X, COIN_TEXT_POS_Y);    
    addText(ssWorld.str(), WORLD_POS_X, WORLD_POS_Y);    
    addText(ssTop.str(), TOP_SCORE_POS_X, TOP_SCORE_POS_Y);

    // Coin Animation
    m_coinSprite.setTexture(m_hudTexture);
    m_coinSprite.setTextureRect(sf::IntRect({COIN_BASE_X, COIN_BASE_Y}, {8, 8}));
    m_coinSprite.setScale({SCALE, SCALE});
    m_coinSprite.setPosition(m_bgSprite.getPosition() + sf::Vector2f(COIN_ICON_POS_X * SCALE, COIN_ICON_POS_Y * SCALE));

    m_fontLoaded = m_font.openFromFile(FONT_PATH);
    if (!m_fontLoaded) {
#ifdef DEBUG
        std::cerr << "[DEBUG][MenuState] Failed to load packaged font from '" << FONT_PATH << "'. Text rendering is disabled.\n";
#endif
    } else {
        m_pressToPlayText.emplace(m_font);
        m_pressToPlayText->setString("PRESS ENTER OR CLICK TO PLAY");
        m_pressToPlayText->setCharacterSize(PRESS_TO_PLAY_FONT_SIZE); // Scaled for logical res
        m_pressToPlayText->setFillColor(GOLD_COLOR); // Retro arcade gold
        m_pressToPlayText->setOutlineColor(sf::Color::Black);
        m_pressToPlayText->setOutlineThickness(OUTLINE_THICKNESS);
        
        sf::FloatRect bounds = m_pressToPlayText->getLocalBounds();
        m_pressToPlayText->setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
        m_pressToPlayText->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, DisplayConfig::LOGICAL_HEIGHT - PRESS_TO_PLAY_OFFSET_Y});

        m_menu = std::make_unique<UIMenuWidget>(m_font);
        m_menu->addItem("START GAME", [this]() {
            if (m_transitioning) return;
            m_transitioning = true;
            GameManager::getInstance().changeState(
                std::make_unique<CharacterSelectState>());
        });
        m_menu->setPosition(
            UILayoutHelper::getAnchorPosition(UIAnchor::BottomCenter) +
                sf::Vector2f(0.f, -45.f),
            UIAnchor::Center);
    }
}

void MenuState::onExit() {}

void MenuState::processEvents(const sf::Event& event) {
    if (!m_transitioning && m_menu) {
        m_menu->processEvents(event);
    }
}

void MenuState::processInput(const InputState& inputState) {
    if (!m_transitioning && m_menu) {
        m_menu->processInput(inputState);
    }
}

void MenuState::update(float dt) {
    if (m_menu) {
        m_menu->update(dt);
    }
    // Coin Animation Logic
    m_coinAnimTimer += dt;
    if (m_coinAnimTimer >= COIN_ANIM_DURATION) {
        m_coinAnimTimer = 0.f;
        m_coinCurrentFrame = (m_coinCurrentFrame + 1) % COIN_FRAMES;
        
        m_coinSprite.setTextureRect(sf::IntRect(
            {COIN_BASE_X + m_coinCurrentFrame * COIN_FRAME_SPACING, COIN_BASE_Y}, 
            {8, 8}
        ));
    }

    // Blinking effect for PRESS TO PLAY text
    m_blinkTimer += dt;
    if (m_blinkTimer >= 0.5f) {
        m_blinkTimer = 0.f;
        m_showPressToPlay = !m_showPressToPlay;
    }
}

void MenuState::render(sf::RenderTarget& target) {
    target.clear(sf::Color(92, 148, 252)); // Classic sky blue
    target.setView(target.getDefaultView());
    
    target.draw(m_bgSprite);
    
    for (const auto& sprite : m_dynamicTextSprites) {
        target.draw(sprite);
    }
    
    target.draw(m_cursorSprite);
    target.draw(m_coinSprite);
    if (m_fontLoaded && m_showPressToPlay && m_pressToPlayText) {
        target.draw(*m_pressToPlayText);
    }
    if (m_menu) {
        m_menu->draw(target);
    }
}
