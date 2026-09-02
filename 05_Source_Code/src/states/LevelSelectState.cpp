/**
 * @file LevelSelectState.cpp
 * @author TV1 (Duong)
 * @brief Clean, focused Level / Stage selection state with rich stage preview screenshots.
 */

#include "states/LevelSelectState.h"

#include "core/DisplayConfig.h"
#include "core/GameManager.h"
#include "core/SoundManager.h"
#include "patterns/InputState.h"
#include "states/CharacterSelectState.h"
#include "states/CoopCharacterSelectState.h"
#include "states/MenuState.h"
#include "ui/UILayoutHelper.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr const char* FONT_PATH = "assets/fonts/mario.ttf";

constexpr const char* STAGE_TEXTURE_PATHS[4] = {
    "assets/textures/ui/stage_1.png",
    "assets/textures/ui/stage_2.png",
    "assets/textures/ui/stage_3.png",
    "assets/textures/ui/stage_4.png"
};

constexpr unsigned int TITLE_FONT_SIZE = 22;
constexpr unsigned int SUBTITLE_FONT_SIZE = 10;
constexpr unsigned int CARD_TITLE_FONT_SIZE = 12;
constexpr unsigned int CARD_THEME_FONT_SIZE = 8;
constexpr unsigned int CARD_DESC_FONT_SIZE = 9;
constexpr unsigned int ACTION_FONT_SIZE = 10;
constexpr unsigned int HINT_FONT_SIZE = 9;

constexpr float TITLE_Y = 16.f;
constexpr float SUBTITLE_Y = 38.f;
constexpr float HINT_Y = 322.f;

constexpr float PANEL_X = 20.f;
constexpr float PANEL_Y = 8.f;
constexpr float PANEL_WIDTH = 600.f;
constexpr float PANEL_HEIGHT = 344.f;

constexpr float CARD_START_X = 35.f;
constexpr float CARD_Y = 58.f;
constexpr float CARD_WIDTH = 135.f;
constexpr float CARD_HEIGHT = 252.f;
constexpr float CARD_SPACING = 10.f;

constexpr float PREVIEW_WIDTH = 121.f;
constexpr float PREVIEW_HEIGHT = 90.f;

const sf::Color SKY_COLOR(18, 26, 56);
const sf::Color PANEL_COLOR(12, 18, 40);
const sf::Color CARD_COLOR(22, 32, 68);
const sf::Color CARD_COLOR_ACTIVE(32, 48, 98);
const sf::Color CARD_OUTLINE(55, 80, 140);
const sf::Color GOLD_COLOR(255, 215, 0);
const sf::Color MUTED_GOLD_COLOR(255, 231, 128);
const sf::Color BODY_COLOR(235, 240, 255);

const sf::Color THEME_BANNER_COLORS[4] = {
    sf::Color(46, 125, 50),   // 1-1 Overworld: Green
    sf::Color(21, 101, 192),  // 1-2 Underground: Cave Blue
    sf::Color(0, 131, 143),   // 1-3 Underwater: Deep Teal
    sf::Color(183, 28, 28)    // 1-4 Castle: Fiery Crimson
};

const sf::Color PREVIEW_FALLBACK_BG[4] = {
    sf::Color(92, 148, 252),  // 1-1: Sky Blue
    sf::Color(0, 0, 0),       // 1-2: Cave Darkness
    sf::Color(32, 56, 236),   // 1-3: Ocean Blue
    sf::Color(18, 8, 12)      // 1-4: Dark Fortress
};

const char* STAGE_DESCRIPTIONS[4] = {
    "GRASSLANDS",
    "UNDERGROUND",
    "DEEP OCEAN",
    "CASTLE KEEP"
};

} // namespace

LevelSelectState::LevelSelectState(Mode mode)
    : m_mode(mode) {}

void LevelSelectState::initStageTextures() {
    m_texturesLoaded = true;
    for (int i = 0; i < 4; ++i) {
        sf::Texture tex;
        if (tex.loadFromFile(STAGE_TEXTURE_PATHS[i])) {
            tex.setSmooth(false);
            m_stageTextures.push_back(std::move(tex));
        } else {
            // Fallback placeholder texture
            sf::Image fallbackImg;
            fallbackImg.resize({240, 180}, PREVIEW_FALLBACK_BG[i % 4]);
            (void)tex.loadFromImage(fallbackImg);
            tex.setSmooth(false);
            m_stageTextures.push_back(std::move(tex));
        }
    }
}

void LevelSelectState::initBackdropPanel() {
    m_panel.setSize({PANEL_WIDTH, PANEL_HEIGHT});
    m_panel.setPosition({PANEL_X, PANEL_Y});
    m_panel.setFillColor(PANEL_COLOR);
    m_panel.setOutlineColor(GOLD_COLOR);
    m_panel.setOutlineThickness(2.5f);
}

void LevelSelectState::initTextLabels() {
    if (!m_fontLoaded) return;

    m_titleText.emplace(m_font, "SELECT WORLD & STAGE", TITLE_FONT_SIZE);
    m_titleText->setFillColor(GOLD_COLOR);
    m_titleText->setOutlineColor(sf::Color::Black);
    m_titleText->setOutlineThickness(1.5f);
    sf::FloatRect titleBounds = m_titleText->getLocalBounds();
    m_titleText->setOrigin({titleBounds.position.x + titleBounds.size.x / 2.f, 0.f});
    m_titleText->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, TITLE_Y});

    if (m_mode == Mode::Coop) {
        m_subtitleText.emplace(m_font, "[ 2 PLAYER CO-OP ] - CHOOSE MISSION STAGE", SUBTITLE_FONT_SIZE);
        m_subtitleText->setFillColor(sf::Color(130, 255, 180));
    } else {
        m_subtitleText.emplace(m_font, "[ 1 PLAYER CAMPAIGN ] - CHOOSE STARTING WORLD", SUBTITLE_FONT_SIZE);
        m_subtitleText->setFillColor(sf::Color(130, 200, 255));
    }
    m_subtitleText->setOutlineColor(sf::Color::Black);
    m_subtitleText->setOutlineThickness(1.f);
    sf::FloatRect subBounds = m_subtitleText->getLocalBounds();
    m_subtitleText->setOrigin({subBounds.position.x + subBounds.size.x / 2.f, 0.f});
    m_subtitleText->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, SUBTITLE_Y});

    m_hintText.emplace(m_font, "[A / D / ARROWS] SELECT    [ENTER / CLICK] START    [ESC] BACK", HINT_FONT_SIZE);
    m_hintText->setFillColor(sf::Color(180, 210, 250));
    sf::FloatRect hintBounds = m_hintText->getLocalBounds();
    m_hintText->setOrigin({hintBounds.position.x + hintBounds.size.x / 2.f, 0.f});
    m_hintText->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, HINT_Y});
}

void LevelSelectState::initStageCards() {
    const auto& catalog = LevelCatalog::getAll();
    const std::size_t count = std::min(catalog.size(), std::size_t(4));

    for (std::size_t i = 0; i < count; ++i) {
        const auto& def = catalog[i];
        StageCard card;
        card.levelNumber = def.number;

        const float cardX = CARD_START_X + static_cast<float>(i) * (CARD_WIDTH + CARD_SPACING);

        // Outer Card Container
        card.outerCard.setSize({CARD_WIDTH, CARD_HEIGHT});
        card.outerCard.setPosition({cardX, CARD_Y});
        card.outerCard.setFillColor(CARD_COLOR);
        card.outerCard.setOutlineColor(CARD_OUTLINE);
        card.outerCard.setOutlineThickness(1.f);

        // Header Banner (Colored by Theme)
        card.headerBanner.setSize({CARD_WIDTH, 34.f});
        card.headerBanner.setPosition({cardX, CARD_Y});
        card.headerBanner.setFillColor(THEME_BANNER_COLORS[i % 4]);

        // Preview Window Box (Aspect ratio ~4:3)
        const float previewX = cardX + 7.f;
        const float previewY = CARD_Y + 40.f;
        card.previewBox.setSize({PREVIEW_WIDTH, PREVIEW_HEIGHT});
        card.previewBox.setPosition({previewX, previewY});
        card.previewBox.setFillColor(PREVIEW_FALLBACK_BG[i % 4]);
        card.previewBox.setOutlineColor(CARD_OUTLINE);
        card.previewBox.setOutlineThickness(1.f);

        // Setup Stage Preview Screenshot Sprite
        if (i < m_stageTextures.size()) {
            card.previewSprite.emplace(m_stageTextures[i]);
            const sf::Vector2u texSize = m_stageTextures[i].getSize();
            if (texSize.x > 0 && texSize.y > 0) {
                card.previewSprite->setScale({
                    PREVIEW_WIDTH / static_cast<float>(texSize.x),
                    PREVIEW_HEIGHT / static_cast<float>(texSize.y)
                });
            }
            card.previewSprite->setPosition({previewX, previewY});
        }

        if (m_fontLoaded) {
            const float centerX = cardX + CARD_WIDTH / 2.f;

            // World Title (e.g. "WORLD 1-1")
            card.titleText.emplace(m_font, "WORLD " + def.worldLabel, CARD_TITLE_FONT_SIZE);
            card.titleText->setFillColor(sf::Color::White);
            card.titleText->setOutlineColor(sf::Color::Black);
            card.titleText->setOutlineThickness(1.f);
            card.titleText->setStyle(sf::Text::Bold);
            sf::FloatRect titleB = card.titleText->getLocalBounds();
            card.titleText->setOrigin({titleB.position.x + titleB.size.x / 2.f, 0.f});
            card.titleText->setPosition({centerX, CARD_Y + 3.f});

            // Theme Name (e.g. "OVERWORLD")
            std::string themeName;
            // The card describes the stage's dominant area.  PlayState still
            // consumes the legacy `theme` field as the initial spawn theme,
            // which is intentionally different for Levels 2 and 3.
            switch (def.dominantTheme) {
                case LevelTheme::OVERWORLD:   themeName = "OVERWORLD"; break;
                case LevelTheme::UNDERGROUND: themeName = "UNDERGROUND"; break;
                case LevelTheme::UNDERWATER:  themeName = "UNDERWATER"; break;
                case LevelTheme::CASTLE:      themeName = "CASTLE"; break;
            }
            card.themeText.emplace(m_font, themeName, CARD_THEME_FONT_SIZE);
            card.themeText->setFillColor(MUTED_GOLD_COLOR);
            card.themeText->setOutlineColor(sf::Color::Black);
            card.themeText->setOutlineThickness(0.8f);
            sf::FloatRect themeB = card.themeText->getLocalBounds();
            card.themeText->setOrigin({themeB.position.x + themeB.size.x / 2.f, 0.f});
            card.themeText->setPosition({centerX, CARD_Y + 19.f});

            // Description Label
            card.descText.emplace(m_font, STAGE_DESCRIPTIONS[i % 4], CARD_DESC_FONT_SIZE);
            card.descText->setFillColor(BODY_COLOR);
            card.descText->setOutlineColor(sf::Color::Black);
            card.descText->setOutlineThickness(1.f);
            sf::FloatRect descB = card.descText->getLocalBounds();
            card.descText->setOrigin({descB.position.x + descB.size.x / 2.f, 0.f});
            card.descText->setPosition({centerX, CARD_Y + 138.f});

            // Status Badge Box & Text
            const float badgeW = 100.f;
            const float badgeH = 18.f;
            card.statusBadge.setSize({badgeW, badgeH});
            card.statusBadge.setPosition({centerX - badgeW / 2.f, CARD_Y + 164.f});
            card.statusBadge.setFillColor(sf::Color(10, 15, 35));
            card.statusBadge.setOutlineThickness(1.f);

            card.statusBadgeText.emplace(m_font, "", 8);
            card.statusBadgeText->setOutlineColor(sf::Color::Black);
            card.statusBadgeText->setOutlineThickness(1.f);

            // High Score Text for this level
            const int stageScore = GameManager::getInstance().getSaveManager().getLevelHighScore(def.number);
            std::string scoreStr = std::to_string(stageScore);
            while (scoreStr.length() < 6) scoreStr = "0" + scoreStr;
            card.highScoreText.emplace(m_font, "BEST: " + scoreStr, 8);
            card.highScoreText->setFillColor(MUTED_GOLD_COLOR);
            card.highScoreText->setOutlineColor(sf::Color::Black);
            card.highScoreText->setOutlineThickness(1.f);
            sf::FloatRect hiB = card.highScoreText->getLocalBounds();
            card.highScoreText->setOrigin({hiB.position.x + hiB.size.x / 2.f, 0.f});
            card.highScoreText->setPosition({centerX, CARD_Y + 192.f});

            // Bottom Action Tag
            card.actionText.emplace(m_font, (i == 0) ? "> PLAY <" : "STAGE " + std::to_string(def.number), ACTION_FONT_SIZE);
            card.actionText->setFillColor((i == 0) ? GOLD_COLOR : MUTED_GOLD_COLOR);
            card.actionText->setOutlineColor(sf::Color::Black);
            card.actionText->setOutlineThickness(1.2f);
            sf::FloatRect actB = card.actionText->getLocalBounds();
            card.actionText->setOrigin({actB.position.x + actB.size.x / 2.f, 0.f});
            card.actionText->setPosition({centerX, CARD_Y + 218.f});
        }

        m_cards.push_back(std::move(card));
    }
}

void LevelSelectState::onEnter() {
    m_transitioning = false;
    m_animTimer = 0.f;
    m_cards.clear();
    m_stageTextures.clear();

    const int levelCount = LevelCatalog::count();
    if (levelCount > 0) {
        m_highestUnlockedLevel = std::clamp(
            GameManager::getInstance().getSaveManager().getData().highestUnlockedLevel,
            1,
            levelCount + 1);
    } else {
        m_highestUnlockedLevel = 0;
    }

    m_fontLoaded = m_font.openFromFile(FONT_PATH);
    if (m_fontLoaded) {
        m_font.setSmooth(false);
    }

    initStageTextures();
    initBackdropPanel();
    initTextLabels();
    initStageCards();

    // Default cursor to the highest unlocked level
    selectCard(std::clamp(m_highestUnlockedLevel - 1, 0, static_cast<int>(m_cards.size()) - 1));
}

void LevelSelectState::onExit() {}

void LevelSelectState::selectCard(int index) {
    if (m_cards.empty()) return;
    m_selectedIndex = (index + static_cast<int>(m_cards.size())) % static_cast<int>(m_cards.size());

    for (std::size_t i = 0; i < m_cards.size(); ++i) {
        const bool active = (static_cast<int>(i) == m_selectedIndex);
        const bool unlocked = isLevelUnlocked(m_cards[i].levelNumber);
        const float centerX = m_cards[i].outerCard.getPosition().x + CARD_WIDTH / 2.f;

        m_cards[i].outerCard.setFillColor(active ? CARD_COLOR_ACTIVE : CARD_COLOR);
        m_cards[i].outerCard.setOutlineColor(active ? GOLD_COLOR : CARD_OUTLINE);
        m_cards[i].outerCard.setOutlineThickness(active ? 2.5f : 1.f);
        m_cards[i].previewBox.setOutlineColor(active ? GOLD_COLOR : CARD_OUTLINE);

        // Update status badge
        if (m_cards[i].statusBadgeText) {
            if (!unlocked) {
                m_cards[i].statusBadge.setOutlineColor(sf::Color(180, 50, 50));
                m_cards[i].statusBadgeText->setString("LOCKED");
                m_cards[i].statusBadgeText->setFillColor(sf::Color(240, 100, 100));
            } else if (m_cards[i].levelNumber < m_highestUnlockedLevel) {
                m_cards[i].statusBadge.setOutlineColor(sf::Color(60, 180, 75));
                m_cards[i].statusBadgeText->setString("CLEARED");
                m_cards[i].statusBadgeText->setFillColor(sf::Color(100, 240, 120));
            } else {
                m_cards[i].statusBadge.setOutlineColor(sf::Color(60, 150, 240));
                m_cards[i].statusBadgeText->setString("UNLOCKED");
                m_cards[i].statusBadgeText->setFillColor(sf::Color(120, 200, 255));
            }
            sf::FloatRect badgB = m_cards[i].statusBadgeText->getLocalBounds();
            m_cards[i].statusBadgeText->setOrigin({badgB.position.x + badgB.size.x / 2.f, 0.f});
            m_cards[i].statusBadgeText->setPosition({centerX, CARD_Y + 168.f});
        }

        // Update action button text
        if (m_cards[i].actionText) {
            if (active) {
                if (unlocked) {
                    m_cards[i].actionText->setString("> START <");
                    m_cards[i].actionText->setFillColor(GOLD_COLOR);
                    m_cards[i].actionText->setStyle(sf::Text::Bold);
                } else {
                    m_cards[i].actionText->setString("LOCKED");
                    m_cards[i].actionText->setFillColor(sf::Color(220, 100, 100));
                    m_cards[i].actionText->setStyle(sf::Text::Bold);
                }
            } else {
                if (unlocked) {
                    m_cards[i].actionText->setString("STAGE " + std::to_string(m_cards[i].levelNumber));
                    m_cards[i].actionText->setFillColor(sf::Color(235, 240, 255));
                } else {
                    m_cards[i].actionText->setString("LOCKED");
                    m_cards[i].actionText->setFillColor(sf::Color(150, 80, 80));
                }
                m_cards[i].actionText->setStyle(sf::Text::Regular);
            }
            sf::FloatRect actB = m_cards[i].actionText->getLocalBounds();
            m_cards[i].actionText->setOrigin({actB.position.x + actB.size.x / 2.f, 0.f});
            m_cards[i].actionText->setPosition({centerX, CARD_Y + 218.f});
        }
    }
}

void LevelSelectState::confirmSelection(int levelNumber) {
    if (m_transitioning) return;

    if (!isLevelUnlocked(levelNumber)) {
        SoundManager::getInstance().playSound(SoundId::BUMP);
        return;
    }

    m_transitioning = true;
    SoundManager::getInstance().playSound(SoundId::COIN);
    if (m_mode == Mode::Coop) {
        GameManager::getInstance().changeState(
            std::make_unique<CoopCharacterSelectState>(levelNumber));
    } else {
        GameManager::getInstance().changeState(
            std::make_unique<CharacterSelectState>(levelNumber));
    }
}

bool LevelSelectState::isLevelUnlocked(int levelNumber) const {
    return LevelCatalog::find(levelNumber) != nullptr &&
           levelNumber >= 1 && levelNumber <= m_highestUnlockedLevel;
}

void LevelSelectState::processEvents(const sf::Event& event) {
    if (m_transitioning) return;

    if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
        const sf::Vector2f logicalPos{
            static_cast<float>(mouseMoved->position.x),
            static_cast<float>(mouseMoved->position.y)};
        for (std::size_t i = 0; i < m_cards.size(); ++i) {
            if (m_cards[i].outerCard.getGlobalBounds().contains(logicalPos)) {
                if (static_cast<int>(i) != m_selectedIndex) {
                    selectCard(static_cast<int>(i));
                }
                break;
            }
        }
    }

    if (const auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseButton->button == sf::Mouse::Button::Left) {
            const sf::Vector2f logicalPos{
                static_cast<float>(mouseButton->position.x),
                static_cast<float>(mouseButton->position.y)};
            for (std::size_t i = 0; i < m_cards.size(); ++i) {
                if (m_cards[i].outerCard.getGlobalBounds().contains(logicalPos)) {
                    selectCard(static_cast<int>(i));
                    confirmSelection(m_cards[i].levelNumber);
                    return;
                }
            }
        }
    }
}

void LevelSelectState::processInput(const InputState& inputState) {
    if (m_transitioning) return;

    if (inputState.wasPressed(sf::Keyboard::Key::Left) ||
        inputState.wasPressed(sf::Keyboard::Key::A)) {
        selectCard(m_selectedIndex - 1);
        SoundManager::getInstance().playSound(SoundId::BUMP);
    } else if (inputState.wasPressed(sf::Keyboard::Key::Right) ||
               inputState.wasPressed(sf::Keyboard::Key::D)) {
        selectCard(m_selectedIndex + 1);
        SoundManager::getInstance().playSound(SoundId::BUMP);
    } else if (inputState.wasPressed(sf::Keyboard::Key::Enter) ||
               inputState.wasPressed(sf::Keyboard::Key::Space)) {
        if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_cards.size())) {
            confirmSelection(m_cards[m_selectedIndex].levelNumber);
        }
    } else if (inputState.wasPressed(sf::Keyboard::Key::Escape)) {
        m_transitioning = true;
        SoundManager::getInstance().playSound(SoundId::POWER_DOWN);
        GameManager::getInstance().changeState(std::make_unique<MenuState>());
    }
}

void LevelSelectState::update(float dt) {
    m_animTimer += dt;

    // Gentle pulse effect for active card outline
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_cards.size())) {
        const float pulse = (std::sin(m_animTimer * 6.f) + 1.f) * 0.5f;
        const sf::Color activeGold(
            static_cast<std::uint8_t>(220 + 35 * pulse),
            static_cast<std::uint8_t>(180 + 35 * pulse),
            0
        );
        m_cards[m_selectedIndex].outerCard.setOutlineColor(activeGold);
    }
}

void LevelSelectState::render(sf::RenderTarget& target) {
    target.clear(SKY_COLOR);
    target.setView(target.getDefaultView());

    target.draw(m_panel);

    if (m_fontLoaded) {
        if (m_titleText) target.draw(*m_titleText);
        if (m_subtitleText) target.draw(*m_subtitleText);
        if (m_hintText) target.draw(*m_hintText);
    }

    for (const auto& card : m_cards) {
        target.draw(card.outerCard);
        target.draw(card.headerBanner);
        target.draw(card.previewBox);

        // Draw Stage Snapshot Screenshot
        if (card.previewSprite) {
            target.draw(*card.previewSprite);
        }

        target.draw(card.statusBadge);

        if (m_fontLoaded) {
            if (card.titleText) target.draw(*card.titleText);
            if (card.themeText) target.draw(*card.themeText);
            if (card.descText) target.draw(*card.descText);
            if (card.statusBadgeText) target.draw(*card.statusBadgeText);
            if (card.highScoreText) target.draw(*card.highScoreText);
            if (card.actionText) target.draw(*card.actionText);
        }
    }
}
