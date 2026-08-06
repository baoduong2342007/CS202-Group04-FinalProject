/**
 * @file MenuState.cpp
 * @author TV2 (Nhật)
 * @brief Sprite-based main menu with bitmap text from HUD spritesheet
 * @note Sprint 5 — replaces text-only MenuState with retro NES-style menu
 */

#include "states/MenuState.h"
#include "states/PlayState.h"
#include "core/GameManager.h"
#include "patterns/InputState.h"

#include <iomanip>
#include <sstream>

#ifdef DEBUG
#include <iostream>
#endif

namespace {
    const float SCALE = 720.f / 224.f; // Scale to fit vertical screen height perfectly
    const sf::IntRect BG_RECT({0, 192}, {256, 224}); // Nestopia scheme (1st column)
    const sf::IntRect CURSOR_RECT({0, 440}, {8, 8}); // Estimated mushroom coords in hud.png

    const float SCREEN_WIDTH = 1280.f;
    const float SCREEN_HEIGHT = 720.f;

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
      m_font(), m_pressToPlayText(m_font),
      m_score(score), m_coins(coins), m_world(world), m_level(level), m_topScore(topScore),
      m_coinSprite(m_hudTexture) {}

void MenuState::onEnter() {
    try {
        m_hudTexture = sf::Texture("assets/textures/ui/hud.png");
    } catch (const std::exception& e) {
#ifdef DEBUG
        std::cerr << "[DEBUG][MenuState] Failed to load hud.png: " << e.what() << "\n";
#endif
        return; // Cannot render menu without texture
    }
    
    // Background
    m_bgSprite.setTexture(m_hudTexture);
    m_bgSprite.setTextureRect(BG_RECT);
    m_bgSprite.setScale(sf::Vector2f(SCALE, SCALE));
    m_bgSprite.setPosition(sf::Vector2f(
        (SCREEN_WIDTH - 256.f * SCALE) / 2.f,
        (SCREEN_HEIGHT - 224.f * SCALE) / 2.f
    ));

    // Cursor (shifted 1 tile right)
    m_cursorSprite.setTexture(m_hudTexture);
    m_cursorSprite.setTextureRect(CURSOR_RECT);
    m_cursorSprite.setScale(sf::Vector2f(SCALE, SCALE));
    m_cursorSprite.setPosition(m_bgSprite.getPosition() + sf::Vector2f(72.f * SCALE, 144.f * SCALE));

    // Bitmap Text (shifted 1 tile right)
    auto addText = [&](const std::string& str, float startX, float startY) {
        for (size_t i = 0; i < str.length(); ++i) {
            sf::Sprite s(m_hudTexture, getCharRect(str[i]));
            s.setScale(sf::Vector2f(SCALE, SCALE));
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
    m_coinSprite.setScale(sf::Vector2f(SCALE, SCALE));
    m_coinSprite.setPosition(m_bgSprite.getPosition() + sf::Vector2f(COIN_ICON_POS_X * SCALE, COIN_ICON_POS_Y * SCALE));

    if (!m_font.openFromFile("assets/fonts/mario.ttf")) {
#ifdef DEBUG
        std::cerr << "[DEBUG][MenuState] Failed to load mario.ttf\n";
#endif
    } else {
        m_pressToPlayText.setString("PRESS ENTER OR CLICK TO PLAY");
        m_pressToPlayText.setCharacterSize(22);
        m_pressToPlayText.setFillColor(sf::Color(255, 215, 0)); // Retro arcade gold
        m_pressToPlayText.setOutlineColor(sf::Color::Black);
        m_pressToPlayText.setOutlineThickness(2.f);
        
        sf::FloatRect bounds = m_pressToPlayText.getLocalBounds();
        m_pressToPlayText.setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
        m_pressToPlayText.setPosition({SCREEN_WIDTH / 2.f, SCREEN_HEIGHT - 45.f});
    }
}

void MenuState::onExit() {}

void MenuState::processEvents(const sf::Event& event) {
    if (m_transitioning) return;

    if (const auto* mouseBtn = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseBtn->button == sf::Mouse::Button::Left) {
            m_transitioning = true;
            GameManager::getInstance().changeState(std::make_unique<PlayState>());
        }
    }
}

void MenuState::processInput(const InputState& inputState) {
    if (!m_transitioning && inputState.wasPressed(sf::Keyboard::Key::Enter)) {
        m_transitioning = true;
        GameManager::getInstance().changeState(std::make_unique<PlayState>());
    }
}

void MenuState::update(float dt) {
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

void MenuState::render(sf::RenderWindow& window) {
    window.clear(sf::Color(92, 148, 252)); // Classic sky blue
    window.setView(window.getDefaultView());
    
    window.draw(m_bgSprite);
    
    for (const auto& sprite : m_dynamicTextSprites) {
        window.draw(sprite);
    }
    
    window.draw(m_cursorSprite);
    window.draw(m_coinSprite);
    if (m_showPressToPlay) {
        window.draw(m_pressToPlayText);
    }
}
