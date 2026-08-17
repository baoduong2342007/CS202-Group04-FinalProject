/**
 * @file MenuState.cpp
 * @author TV1 (Dương) & TV2 (Nhật)
 * @brief Polished, crisp retro-modern Title / Main Menu state
 */

#include "states/MenuState.h"

#include "core/DisplayConfig.h"
#include "core/GameManager.h"
#include "core/SoundManager.h"
#include "core/SpriteFrames_ovw.h"
#include "core/SpriteFrames_shared.h"
#include "level/TileFrames.h"
#include "patterns/InputState.h"
#include "states/LevelSelectState.h"
#include "states/PvpCharacterSelectState.h"
#include "ui/UILayoutHelper.h"

#include <cmath>

namespace {
constexpr const char* FONT_PATH = "assets/fonts/mario.ttf";
constexpr const char* TILESET_PATH = "assets/textures/tiles/tileset.png";
constexpr const char* CHARACTER_PATH = "assets/textures/mario/MarioLuigi.png";
constexpr const char* ITEMS_PATH = "assets/textures/items/items_objects.png";
constexpr const char* ENEMIES_PATH = "assets/textures/enemies/enemies.png";

const sf::Color SKY_COLOR(92, 148, 252);
const sf::Color GOLD_COLOR(255, 215, 0);
const sf::Color SHADOW_COLOR(180, 20, 20);
const sf::Color MUTED_GOLD_COLOR(255, 231, 128);

constexpr float TILE_SCALE = 2.0f; // 16px * 2 = 32px per tile
constexpr float SPRITE_SCALE = 2.0f;
} // namespace

MenuState::MenuState(int score, int coins, int world, int level, int topScore)
    : m_score(score), m_coins(coins), m_world(world), m_level(level), m_topScore(topScore) {}

void MenuState::initTextures() {
    m_texturesLoaded = true;

    auto loadSafe = [this](sf::Texture& tex, const char* path) {
        if (tex.loadFromFile(path)) {
            tex.setSmooth(false);
        } else {
            m_texturesLoaded = false;
        }
    };

    loadSafe(m_tilesetTexture, TILESET_PATH);
    loadSafe(m_characterTexture, CHARACTER_PATH);
    loadSafe(m_itemsTexture, ITEMS_PATH);
    loadSafe(m_enemiesTexture, ENEMIES_PATH);
}

void MenuState::initScenery() {
    m_scenerySprites.clear();
    if (!m_texturesLoaded) return;

    // Ground tiles along the bottom (2 rows: y=328 and y=344)
    for (int col = 0; col < 20; ++col) {
        float x = static_cast<float>(col * 32);
        
        // Row 1 (top ground tile)
        sf::Sprite groundTop(m_tilesetTexture, TileFrames::GROUND);
        groundTop.setScale({TILE_SCALE, TILE_SCALE});
        groundTop.setPosition({x, 328.f});
        m_scenerySprites.push_back(groundTop);

        // Row 2 (bottom ground tile)
        sf::Sprite groundBot(m_tilesetTexture, TileFrames::GROUND);
        groundBot.setScale({TILE_SCALE, TILE_SCALE});
        groundBot.setPosition({x, 344.f});
        m_scenerySprites.push_back(groundBot);
    }

    // Left Decorative Pipe at x=32, y=264
    sf::Sprite pipeLeftTopL(m_tilesetTexture, TileFrames::PIPE_TOP_LEFT);
    pipeLeftTopL.setScale({TILE_SCALE, TILE_SCALE});
    pipeLeftTopL.setPosition({32.f, 264.f});
    m_scenerySprites.push_back(pipeLeftTopL);

    sf::Sprite pipeLeftTopR(m_tilesetTexture, TileFrames::PIPE_TOP_RIGHT);
    pipeLeftTopR.setScale({TILE_SCALE, TILE_SCALE});
    pipeLeftTopR.setPosition({64.f, 264.f});
    m_scenerySprites.push_back(pipeLeftTopR);

    sf::Sprite pipeLeftBodyL(m_tilesetTexture, TileFrames::PIPE_BODY_LEFT);
    pipeLeftBodyL.setScale({TILE_SCALE, TILE_SCALE});
    pipeLeftBodyL.setPosition({32.f, 296.f});
    m_scenerySprites.push_back(pipeLeftBodyL);

    sf::Sprite pipeLeftBodyR(m_tilesetTexture, TileFrames::PIPE_BODY_RIGHT);
    pipeLeftBodyR.setScale({TILE_SCALE, TILE_SCALE});
    pipeLeftBodyR.setPosition({64.f, 296.f});
    m_scenerySprites.push_back(pipeLeftBodyR);

    // Right Decorative Pipe at x=544, y=264 (symmetric with left pipe at x=32)
    sf::Sprite pipeRightTopL(m_tilesetTexture, TileFrames::PIPE_TOP_LEFT);
    pipeRightTopL.setScale({TILE_SCALE, TILE_SCALE});
    pipeRightTopL.setPosition({544.f, 264.f});
    m_scenerySprites.push_back(pipeRightTopL);

    sf::Sprite pipeRightTopR(m_tilesetTexture, TileFrames::PIPE_TOP_RIGHT);
    pipeRightTopR.setScale({TILE_SCALE, TILE_SCALE});
    pipeRightTopR.setPosition({576.f, 264.f});
    m_scenerySprites.push_back(pipeRightTopR);

    sf::Sprite pipeRightBodyL(m_tilesetTexture, TileFrames::PIPE_BODY_LEFT);
    pipeRightBodyL.setScale({TILE_SCALE, TILE_SCALE});
    pipeRightBodyL.setPosition({544.f, 296.f});
    m_scenerySprites.push_back(pipeRightBodyL);

    sf::Sprite pipeRightBodyR(m_tilesetTexture, TileFrames::PIPE_BODY_RIGHT);
    pipeRightBodyR.setScale({TILE_SCALE, TILE_SCALE});
    pipeRightBodyR.setPosition({576.f, 296.f});
    m_scenerySprites.push_back(pipeRightBodyR);

    // Mario Sprite on the left (symmetric at x=184)
    m_marioSprite.emplace(m_characterTexture, SpriteFrames::shared::SmallMario::WALK1);
    m_marioSprite->setScale({SPRITE_SCALE, SPRITE_SCALE});
    m_marioSprite->setPosition({184.f, 296.f});

    // Luigi Sprite on the right (symmetric at x=424)
    m_luigiSprite.emplace(m_characterTexture, SpriteFrames::shared::SmallLuigi::IDLE);
    m_luigiSprite->setScale({SPRITE_SCALE, SPRITE_SCALE});
    m_luigiSprite->setPosition({424.f, 296.f});

    // Goomba Sprite in the exact center (x=304)
    m_goombaSprite.emplace(m_enemiesTexture, SpriteFrames::ovw::Enemies::Goomba::WALK1);
    m_goombaSprite->setScale({SPRITE_SCALE, SPRITE_SCALE});
    m_goombaSprite->setPosition({304.f, 296.f});

    // Cursor Mushroom
    m_cursorMushroomSprite.emplace(m_itemsTexture, SpriteFrames::ovw::Items::SUPER_MUSHROOM);
    m_cursorMushroomSprite->setScale({1.5f, 1.5f});
}

void MenuState::initTitleBanner() {
    if (!m_fontLoaded) return;

    // Main 3D Title Shadow
    m_titleShadowText.emplace(m_font, "SUPER MARIO BROS", 34);
    m_titleShadowText->setFillColor(SHADOW_COLOR);
    sf::FloatRect shadowB = m_titleShadowText->getLocalBounds();
    m_titleShadowText->setOrigin({shadowB.position.x + shadowB.size.x / 2.f, 0.f});
    m_titleShadowText->setPosition({322.f, 44.f});

    // Main Title Text
    m_titleMainText.emplace(m_font, "SUPER MARIO BROS", 34);
    m_titleMainText->setFillColor(GOLD_COLOR);
    m_titleMainText->setOutlineColor(sf::Color::Black);
    m_titleMainText->setOutlineThickness(2.0f);
    sf::FloatRect titleB = m_titleMainText->getLocalBounds();
    m_titleMainText->setOrigin({titleB.position.x + titleB.size.x / 2.f, 0.f});
    m_titleMainText->setPosition({320.f, 42.f});
}

void MenuState::initCredits() {
    if (!m_fontLoaded) return;

    // Top Bar - Course / Project Label
    m_courseText.emplace(m_font, "CS202 OOP PROJECT", 11);
    m_courseText->setFillColor(sf::Color::White);
    m_courseText->setOutlineColor(sf::Color::Black);
    m_courseText->setOutlineThickness(1.2f);
    m_courseText->setPosition({24.f, 12.f});

    // Top Bar - Team Name
    m_groupText.emplace(m_font, "GROUP 04", 11);
    m_groupText->setFillColor(GOLD_COLOR);
    m_groupText->setOutlineColor(sf::Color::Black);
    m_groupText->setOutlineThickness(1.2f);
    sf::FloatRect groupB = m_groupText->getLocalBounds();
    m_groupText->setOrigin({groupB.position.x + groupB.size.x, 0.f});
    m_groupText->setPosition({616.f, 12.f});

    // Prompt Text
    m_promptText.emplace(m_font, "[ PRESS ENTER OR CLICK TO START ]", 10);
    m_promptText->setFillColor(GOLD_COLOR);
    m_promptText->setOutlineColor(sf::Color::Black);
    m_promptText->setOutlineThickness(1.5f);
    sf::FloatRect promptB = m_promptText->getLocalBounds();
    m_promptText->setOrigin({promptB.position.x + promptB.size.x / 2.f, 0.f});
    m_promptText->setPosition({320.f, 240.f});

    // Copyright Text
    m_copyrightText.emplace(m_font, "(C) 1985 NINTENDO   CS202-FINAL", 8);
    m_copyrightText->setFillColor(sf::Color(180, 210, 250));
    m_copyrightText->setOutlineColor(sf::Color::Black);
    m_copyrightText->setOutlineThickness(1.f);
    sf::FloatRect copyB = m_copyrightText->getLocalBounds();
    m_copyrightText->setOrigin({copyB.position.x + copyB.size.x / 2.f, 0.f});
    m_copyrightText->setPosition({320.f, 260.f});
}

void MenuState::initMenu() {
    if (!m_fontLoaded) return;

    m_menu = std::make_unique<UIMenuWidget>(m_font);
    m_menu->addItem("1 PLAYER GAME", [this]() {
        if (m_transitioning) return;
        m_transitioning = true;
        SoundManager::getInstance().playSound("coin");
        GameManager::getInstance().changeState(std::make_unique<LevelSelectState>());
    }, 13);
    m_menu->addItem("2 PLAYER CO-OP", [this]() {
        if (m_transitioning) return;
        m_transitioning = true;
        SoundManager::getInstance().playSound("coin");
        GameManager::getInstance().changeState(
            std::make_unique<LevelSelectState>(LevelSelectState::Mode::Coop));
    }, 13);
    m_menu->addItem("2 PLAYER VERSUS", [this]() {
        if (m_transitioning) return;
        m_transitioning = true;
        SoundManager::getInstance().playSound("coin");
        GameManager::getInstance().changeState(std::make_unique<PvpCharacterSelectState>());
    }, 13);

    m_menu->setSpacing(24.f);
    m_menu->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, 178.f}, UIAnchor::Center);
}

void MenuState::onEnter() {
    m_topScore = GameManager::getInstance().getSaveManager().getData().highScore;
    m_transitioning = false;
    m_animTimer = 0.f;
    m_blinkTimer = 0.f;

    initTextures();

    m_fontLoaded = m_font.openFromFile(FONT_PATH);
    if (m_fontLoaded) {
        m_font.setSmooth(false);
    }

    initScenery();
    initTitleBanner();
    initCredits();
    initMenu();
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
    m_animTimer += dt;
    m_blinkTimer += dt;

    if (m_menu) {
        m_menu->update(dt);
    }

    // Blink prompt every 0.45s
    if (m_blinkTimer >= 0.45f) {
        m_blinkTimer = 0.f;
        m_showPrompt = !m_showPrompt;
    }

    // Float Title Banner with subtle bobbing
    const float floatY = std::sin(m_animTimer * 2.5f) * 2.5f;
    if (m_titleShadowText) m_titleShadowText->setPosition({322.f, 48.f + floatY});
    if (m_titleMainText) m_titleMainText->setPosition({320.f, 46.f + floatY});

    // Update Mario, Luigi, Goomba animation frames
    if (m_texturesLoaded) {
        const int marioFrame = static_cast<int>(m_animTimer / 0.16f) % 3;
        static const sf::IntRect marioWalk[3] = {
            SpriteFrames::shared::SmallMario::WALK1,
            SpriteFrames::shared::SmallMario::WALK2,
            SpriteFrames::shared::SmallMario::WALK3
        };
        if (m_marioSprite) {
            m_marioSprite->setTextureRect(marioWalk[marioFrame]);
        }

        const int luigiFrame = static_cast<int>(m_animTimer / 0.22f) % 2;
        if (m_luigiSprite) {
            m_luigiSprite->setTextureRect(luigiFrame == 0 ? SpriteFrames::shared::SmallLuigi::IDLE
                                                           : SpriteFrames::shared::SmallLuigi::JUMP);
        }

        const int goombaFrame = static_cast<int>(m_animTimer / 0.2f) % 2;
        if (m_goombaSprite) {
            m_goombaSprite->setTextureRect(goombaFrame == 0 ? SpriteFrames::ovw::Enemies::Goomba::WALK1
                                                            : SpriteFrames::ovw::Enemies::Goomba::WALK2);
        }
    }

    // Update cursor position beside selected menu item
    if (m_menu && m_cursorMushroomSprite) {
        const int selectedIdx = m_menu->getSelectedIndex();
        if (auto bounds = m_menu->getItemBounds(selectedIdx)) {
            m_cursorMushroomSprite->setPosition({bounds->position.x - 28.f, bounds->position.y - 2.f});
        }
    }
}

void MenuState::render(sf::RenderTarget& target) {
    target.clear(SKY_COLOR);
    target.setView(target.getDefaultView());

    // Draw static scenery and terrain
    for (const auto& sprite : m_scenerySprites) {
        target.draw(sprite);
    }

    // Draw live animated characters
    if (m_marioSprite) target.draw(*m_marioSprite);
    if (m_luigiSprite) target.draw(*m_luigiSprite);
    if (m_goombaSprite) target.draw(*m_goombaSprite);

    // Draw Title Plaque
    if (m_titleShadowText) target.draw(*m_titleShadowText);
    if (m_titleMainText) target.draw(*m_titleMainText);

    // Draw Top Bar & Copyright Credits
    if (m_courseText) target.draw(*m_courseText);
    if (m_groupText) target.draw(*m_groupText);
    if (m_copyrightText) target.draw(*m_copyrightText);

    // Draw Interactive Menu & Cursor
    if (m_menu) {
        m_menu->draw(target);
    }
    if (m_cursorMushroomSprite) {
        target.draw(*m_cursorMushroomSprite);
    }

    // Draw Prompt
    if (m_showPrompt && m_promptText) {
        target.draw(*m_promptText);
    }
}
