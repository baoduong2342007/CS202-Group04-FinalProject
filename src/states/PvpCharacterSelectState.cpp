/**
 * @file PvpCharacterSelectState.cpp
 * @author TV5 (Truyền)
 * @brief Sequential two-player character selection implementation for PvP duel.
 */

#include "states/PvpCharacterSelectState.h"

#include "core/DisplayConfig.h"
#include "core/GameManager.h"
#include "core/SoundManager.h"
#include "core/SpriteFrames_shared.h"
#include "patterns/InputState.h"
#include "states/MenuState.h"
#include "states/PvpPlayState.h"
#include "ui/UILayoutHelper.h"

#include <cmath>

namespace {
constexpr const char* FONT_PATH = "assets/fonts/mario.ttf";
constexpr const char* CHARACTER_TEXTURE_PATH = "assets/textures/mario/MarioLuigi.png";

constexpr unsigned int TITLE_FONT_SIZE = 22;
constexpr unsigned int SUBTITLE_FONT_SIZE = 10;
constexpr unsigned int HERO_NAME_FONT_SIZE = 14;
constexpr unsigned int ROLE_FONT_SIZE = 8;
constexpr unsigned int STATS_FONT_SIZE = 8;
constexpr unsigned int ACTION_FONT_SIZE = 11;
constexpr unsigned int BADGE_FONT_SIZE = 9;
constexpr unsigned int HINT_FONT_SIZE = 8;

constexpr float TITLE_Y = 14.f;
constexpr float SUBTITLE_Y = 36.f;
constexpr float HINT_Y = 320.f;
constexpr float CONFIRM_SPLASH_SECONDS = 1.2f;

constexpr float PANEL_X = 45.f;
constexpr float PANEL_Y = 14.f;
constexpr float PANEL_WIDTH = 550.f;
constexpr float PANEL_HEIGHT = 332.f;

constexpr float CARD_Y = 58.f;
constexpr float CARD_WIDTH = 240.f;
constexpr float CARD_HEIGHT = 252.f;
constexpr float MARIO_CARD_X = 65.f;
constexpr float LUIGI_CARD_X = 335.f;

constexpr float AVATAR_BOX_W = 104.f;
constexpr float AVATAR_BOX_H = 72.f;
constexpr float HERO_SCALE = 2.0f;

const sf::Color SKY_COLOR(28, 42, 82);
const sf::Color PANEL_COLOR(17, 24, 52);
const sf::Color CARD_COLOR(22, 32, 68);
const sf::Color CARD_COLOR_ACTIVE(30, 46, 94);
const sf::Color CARD_OUTLINE(55, 80, 140);
const sf::Color GOLD_COLOR(255, 215, 0);
const sf::Color MUTED_GOLD_COLOR(255, 231, 128);
const sf::Color AVATAR_BOX_BG(10, 15, 36);

const sf::Color P1_COLOR(100, 200, 255);
const sf::Color P2_COLOR(255, 120, 120);

const sf::Color MARIO_THEME_COLOR(185, 28, 28);
const sf::Color LUIGI_THEME_COLOR(27, 130, 48);

const char* characterName(CharacterType type) {
    return type == CharacterType::LUIGI ? "LUIGI" : "MARIO";
}
} // namespace

void PvpCharacterSelectState::initBackdropPanel() {
    m_panel.setSize({PANEL_WIDTH, PANEL_HEIGHT});
    m_panel.setPosition({PANEL_X, PANEL_Y});
    m_panel.setFillColor(PANEL_COLOR);
    m_panel.setOutlineColor(GOLD_COLOR);
    m_panel.setOutlineThickness(-2.0f);
}

void PvpCharacterSelectState::initCards() {
    // Mario Card (Left)
    m_marioCard.setSize({CARD_WIDTH, CARD_HEIGHT});
    m_marioCard.setPosition({MARIO_CARD_X, CARD_Y});
    m_marioCard.setFillColor(CARD_COLOR_ACTIVE);
    m_marioCard.setOutlineColor(GOLD_COLOR);
    m_marioCard.setOutlineThickness(2.5f);

    m_marioHeader.setSize({CARD_WIDTH, 36.f});
    m_marioHeader.setPosition({MARIO_CARD_X, CARD_Y});
    m_marioHeader.setFillColor(MARIO_THEME_COLOR);

    m_marioAvatarBox.setSize({AVATAR_BOX_W, AVATAR_BOX_H});
    m_marioAvatarBox.setPosition({MARIO_CARD_X + (CARD_WIDTH - AVATAR_BOX_W) / 2.f, CARD_Y + 44.f});
    m_marioAvatarBox.setFillColor(AVATAR_BOX_BG);
    m_marioAvatarBox.setOutlineColor(GOLD_COLOR);
    m_marioAvatarBox.setOutlineThickness(1.5f);

    // Luigi Card (Right)
    m_luigiCard.setSize({CARD_WIDTH, CARD_HEIGHT});
    m_luigiCard.setPosition({LUIGI_CARD_X, CARD_Y});
    m_luigiCard.setFillColor(CARD_COLOR);
    m_luigiCard.setOutlineColor(CARD_OUTLINE);
    m_luigiCard.setOutlineThickness(1.f);

    m_luigiHeader.setSize({CARD_WIDTH, 36.f});
    m_luigiHeader.setPosition({LUIGI_CARD_X, CARD_Y});
    m_luigiHeader.setFillColor(LUIGI_THEME_COLOR);

    m_luigiAvatarBox.setSize({AVATAR_BOX_W, AVATAR_BOX_H});
    m_luigiAvatarBox.setPosition({LUIGI_CARD_X + (CARD_WIDTH - AVATAR_BOX_W) / 2.f, CARD_Y + 44.f});
    m_luigiAvatarBox.setFillColor(AVATAR_BOX_BG);
    m_luigiAvatarBox.setOutlineColor(CARD_OUTLINE);
    m_luigiAvatarBox.setOutlineThickness(1.f);

    // Player Badges
    m_p1BadgeBox.setSize({46.f, 18.f});
    m_p1BadgeBox.setFillColor(sf::Color(10, 35, 75));
    m_p1BadgeBox.setOutlineColor(P1_COLOR);
    m_p1BadgeBox.setOutlineThickness(1.5f);

    m_p2BadgeBox.setSize({46.f, 18.f});
    m_p2BadgeBox.setFillColor(sf::Color(60, 15, 15));
    m_p2BadgeBox.setOutlineColor(P2_COLOR);
    m_p2BadgeBox.setOutlineThickness(1.5f);
}

void PvpCharacterSelectState::initAvatars() {
    m_characterTextureLoaded = false;
    try {
        m_characterTexture.emplace(CHARACTER_TEXTURE_PATH);
        m_characterTextureLoaded = true;
    } catch (const std::exception&) {
        m_characterTexture.reset();
        m_characterTextureLoaded = false;
    }

    if (m_characterTextureLoaded && m_characterTexture) {
        m_characterTexture->setSmooth(false);

        m_marioSprite.emplace(*m_characterTexture);
        m_marioSprite->setTextureRect(SpriteFrames::shared::BigMario::WALK1);
        m_marioSprite->setScale({HERO_SCALE, HERO_SCALE});
        m_marioSprite->setPosition({
            MARIO_CARD_X + (CARD_WIDTH - 16.f * HERO_SCALE) / 2.f,
            CARD_Y + 44.f + (AVATAR_BOX_H - 32.f * HERO_SCALE) / 2.f
        });

        m_luigiSprite.emplace(*m_characterTexture);
        m_luigiSprite->setTextureRect(SpriteFrames::shared::BigLuigi::WALK1);
        m_luigiSprite->setScale({HERO_SCALE, HERO_SCALE});
        m_luigiSprite->setPosition({
            LUIGI_CARD_X + (CARD_WIDTH - 16.f * HERO_SCALE) / 2.f,
            CARD_Y + 44.f + (AVATAR_BOX_H - 32.f * HERO_SCALE) / 2.f
        });
    }
}

void PvpCharacterSelectState::initTextLabels() {
    if (!m_fontLoaded) return;

    m_titleText.emplace(m_font, "2 PLAYER VERSUS", TITLE_FONT_SIZE);
    m_titleText->setFillColor(GOLD_COLOR);
    m_titleText->setOutlineColor(sf::Color::Black);
    m_titleText->setOutlineThickness(1.5f);
    sf::FloatRect titleB = m_titleText->getLocalBounds();
    m_titleText->setOrigin({titleB.position.x + titleB.size.x / 2.f, 0.f});
    m_titleText->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, TITLE_Y});

    m_subtitleText.emplace(m_font, "PLAYER 1 - CHOOSE YOUR FIGHTER!", SUBTITLE_FONT_SIZE);
    m_subtitleText->setFillColor(P1_COLOR);
    m_subtitleText->setOutlineColor(sf::Color::Black);
    m_subtitleText->setOutlineThickness(1.f);
    sf::FloatRect subB = m_subtitleText->getLocalBounds();
    m_subtitleText->setOrigin({subB.position.x + subB.size.x / 2.f, 0.f});
    m_subtitleText->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, SUBTITLE_Y});

    const float marioCenterX = MARIO_CARD_X + CARD_WIDTH / 2.f;
    const float luigiCenterX = LUIGI_CARD_X + CARD_WIDTH / 2.f;

    m_marioNameText.emplace(m_font, "MARIO", HERO_NAME_FONT_SIZE);
    m_marioNameText->setFillColor(sf::Color::White);
    m_marioNameText->setOutlineColor(sf::Color::Black);
    m_marioNameText->setOutlineThickness(1.f);
    m_marioNameText->setStyle(sf::Text::Bold);
    sf::FloatRect mNameB = m_marioNameText->getLocalBounds();
    m_marioNameText->setOrigin({mNameB.position.x + mNameB.size.x / 2.f, 0.f});
    m_marioNameText->setPosition({marioCenterX, CARD_Y + 3.f});

    m_marioRoleText.emplace(m_font, "THE BALANCED RUNNER", ROLE_FONT_SIZE);
    m_marioRoleText->setFillColor(MUTED_GOLD_COLOR);
    m_marioRoleText->setOutlineColor(sf::Color::Black);
    m_marioRoleText->setOutlineThickness(0.8f);
    sf::FloatRect mRoleB = m_marioRoleText->getLocalBounds();
    m_marioRoleText->setOrigin({mRoleB.position.x + mRoleB.size.x / 2.f, 0.f});
    m_marioRoleText->setPosition({marioCenterX, CARD_Y + 20.f});

    m_marioStatsText.emplace(m_font,
        "SPEED     [====---]  280\n"
        "JUMP      [===----]  460\n"
        "TRACTION  [======-]  MAX",
        STATS_FONT_SIZE);
    m_marioStatsText->setFillColor(MUTED_GOLD_COLOR);
    m_marioStatsText->setOutlineColor(sf::Color::Black);
    m_marioStatsText->setOutlineThickness(1.f);
    m_marioStatsText->setLineSpacing(1.4f);
    sf::FloatRect mStatsB = m_marioStatsText->getLocalBounds();
    m_marioStatsText->setPosition({
        MARIO_CARD_X + (CARD_WIDTH - mStatsB.size.x) / 2.f - mStatsB.position.x,
        CARD_Y + 126.f
    });

    m_marioActionText.emplace(m_font, "> CHOOSE MARIO <", ACTION_FONT_SIZE);
    m_marioActionText->setFillColor(GOLD_COLOR);
    m_marioActionText->setOutlineColor(sf::Color::Black);
    m_marioActionText->setOutlineThickness(1.5f);
    m_marioActionText->setStyle(sf::Text::Bold);
    sf::FloatRect mActB = m_marioActionText->getLocalBounds();
    m_marioActionText->setOrigin({mActB.position.x + mActB.size.x / 2.f, 0.f});
    m_marioActionText->setPosition({marioCenterX, CARD_Y + 218.f});

    m_luigiNameText.emplace(m_font, "LUIGI", HERO_NAME_FONT_SIZE);
    m_luigiNameText->setFillColor(sf::Color::White);
    m_luigiNameText->setOutlineColor(sf::Color::Black);
    m_luigiNameText->setOutlineThickness(1.f);
    m_luigiNameText->setStyle(sf::Text::Bold);
    sf::FloatRect lNameB = m_luigiNameText->getLocalBounds();
    m_luigiNameText->setOrigin({lNameB.position.x + lNameB.size.x / 2.f, 0.f});
    m_luigiNameText->setPosition({luigiCenterX, CARD_Y + 3.f});

    m_luigiRoleText.emplace(m_font, "THE HIGH JUMPER", ROLE_FONT_SIZE);
    m_luigiRoleText->setFillColor(MUTED_GOLD_COLOR);
    m_luigiRoleText->setOutlineColor(sf::Color::Black);
    m_luigiRoleText->setOutlineThickness(0.8f);
    sf::FloatRect lRoleB = m_luigiRoleText->getLocalBounds();
    m_luigiRoleText->setOrigin({lRoleB.position.x + lRoleB.size.x / 2.f, 0.f});
    m_luigiRoleText->setPosition({luigiCenterX, CARD_Y + 20.f});

    m_luigiStatsText.emplace(m_font,
        "SPEED     [===----]  250\n"
        "JUMP      [======-]  510\n"
        "TRACTION  [====---]  MED",
        STATS_FONT_SIZE);
    m_luigiStatsText->setFillColor(MUTED_GOLD_COLOR);
    m_luigiStatsText->setOutlineColor(sf::Color::Black);
    m_luigiStatsText->setOutlineThickness(1.f);
    m_luigiStatsText->setLineSpacing(1.4f);
    sf::FloatRect lStatsB = m_luigiStatsText->getLocalBounds();
    m_luigiStatsText->setPosition({
        LUIGI_CARD_X + (CARD_WIDTH - lStatsB.size.x) / 2.f - lStatsB.position.x,
        CARD_Y + 126.f
    });

    m_luigiActionText.emplace(m_font, "CHOOSE LUIGI", ACTION_FONT_SIZE);
    m_luigiActionText->setFillColor(MUTED_GOLD_COLOR);
    m_luigiActionText->setOutlineColor(sf::Color::Black);
    m_luigiActionText->setOutlineThickness(1.2f);
    sf::FloatRect lActB = m_luigiActionText->getLocalBounds();
    m_luigiActionText->setOrigin({lActB.position.x + lActB.size.x / 2.f, 0.f});
    m_luigiActionText->setPosition({luigiCenterX, CARD_Y + 218.f});

    // P1 / P2 Badges Text
    m_p1BadgeText.emplace(m_font, "P1", BADGE_FONT_SIZE);
    m_p1BadgeText->setFillColor(P1_COLOR);
    m_p1BadgeText->setOutlineColor(sf::Color::Black);
    m_p1BadgeText->setOutlineThickness(1.f);
    m_p1BadgeText->setStyle(sf::Text::Bold);

    m_p2BadgeText.emplace(m_font, "P2", BADGE_FONT_SIZE);
    m_p2BadgeText->setFillColor(P2_COLOR);
    m_p2BadgeText->setOutlineColor(sf::Color::Black);
    m_p2BadgeText->setOutlineThickness(1.f);
    m_p2BadgeText->setStyle(sf::Text::Bold);

    m_hintText.emplace(m_font, "[A / D] SELECT    [ENTER] CONFIRM    [ESC] BACK", HINT_FONT_SIZE);
    m_hintText->setFillColor(sf::Color(180, 210, 250));
    sf::FloatRect hintB = m_hintText->getLocalBounds();
    m_hintText->setOrigin({hintB.position.x + hintB.size.x / 2.f, 0.f});
    m_hintText->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, HINT_Y});
}

void PvpCharacterSelectState::initMenu() {
    rebuildMenuForPhase();
}

void PvpCharacterSelectState::rebuildMenuForPhase() {
    if (m_subtitleText) {
        if (m_phase == Phase::PLAYER_ONE) {
            m_subtitleText->setString("PLAYER 1 - CHOOSE FIGHTER!");
            m_subtitleText->setFillColor(P1_COLOR);
        } else if (m_phase == Phase::PLAYER_TWO) {
            m_subtitleText->setString("PLAYER 2 - CHOOSE FIGHTER!");
            m_subtitleText->setFillColor(P2_COLOR);
        } else {
            std::string splash = "P1 " + std::string(characterName(m_playerOneChoice)) +
                                 "  VS  " + std::string(characterName(m_playerTwoChoice)) + " P2 - DUEL!";
            m_subtitleText->setString(splash);
            m_subtitleText->setFillColor(GOLD_COLOR);
        }
        sf::FloatRect subB = m_subtitleText->getLocalBounds();
        m_subtitleText->setOrigin({subB.position.x + subB.size.x / 2.f, 0.f});
        m_subtitleText->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, SUBTITLE_Y});
    }
}

void PvpCharacterSelectState::confirmChoice(CharacterType characterType) {
    if (m_transitioning) return;

    SoundManager::getInstance().playSound(SoundId::COIN);

    if (m_phase == Phase::PLAYER_ONE) {
        m_playerOneChoice = characterType;
        m_phase = Phase::PLAYER_TWO;
        m_selectedIndex = 0;
        rebuildMenuForPhase();
    } else if (m_phase == Phase::PLAYER_TWO) {
        m_playerTwoChoice = characterType;
        m_phase = Phase::CONFIRM;
        m_confirmTimer = CONFIRM_SPLASH_SECONDS;
        rebuildMenuForPhase();
    }
}

void PvpCharacterSelectState::queueMatch() {
    if (m_transitioning) return;

    m_transitioning = true;
    GameManager::getInstance().changeState(
        std::make_unique<PvpPlayState>(m_playerOneChoice, m_playerTwoChoice));
}

void PvpCharacterSelectState::onEnter() {
    m_transitioning = false;
    m_phase = Phase::PLAYER_ONE;
    m_animTimer = 0.f;
    m_confirmTimer = 0.f;
    m_selectedIndex = 0;

    initBackdropPanel();
    initCards();
    initAvatars();

    m_fontLoaded = m_font.openFromFile(FONT_PATH);
    if (m_fontLoaded) {
        m_font.setSmooth(false);
    }
    initTextLabels();
    initMenu();
}

void PvpCharacterSelectState::onExit() {}

void PvpCharacterSelectState::processEvents(const sf::Event& event) {
    if (m_transitioning || m_phase == Phase::CONFIRM) return;

    if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
        const sf::Vector2f pos{static_cast<float>(mouseMoved->position.x),
                               static_cast<float>(mouseMoved->position.y)};
        if (m_marioCard.getGlobalBounds().contains(pos)) {
            m_selectedIndex = 0;
        } else if (m_luigiCard.getGlobalBounds().contains(pos)) {
            m_selectedIndex = 1;
        }
    }

    if (const auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseButton->button == sf::Mouse::Button::Left) {
            const sf::Vector2f pos{static_cast<float>(mouseButton->position.x),
                                   static_cast<float>(mouseButton->position.y)};
            if (m_marioCard.getGlobalBounds().contains(pos)) {
                confirmChoice(CharacterType::MARIO);
            } else if (m_luigiCard.getGlobalBounds().contains(pos)) {
                confirmChoice(CharacterType::LUIGI);
            }
        }
    }
}

void PvpCharacterSelectState::processInput(const InputState& inputState) {
    if (m_transitioning) return;

    if (inputState.wasPressed(sf::Keyboard::Key::Escape)) {
        if (m_phase == Phase::PLAYER_TWO) {
            m_phase = Phase::PLAYER_ONE;
            m_selectedIndex = (m_playerOneChoice == CharacterType::MARIO) ? 0 : 1;
            rebuildMenuForPhase();
        } else if (m_phase == Phase::PLAYER_ONE) {
            m_transitioning = true;
            SoundManager::getInstance().playSound(SoundId::POWER_DOWN);
            GameManager::getInstance().changeState(std::make_unique<MenuState>());
        }
        return;
    }

    if (m_phase == Phase::CONFIRM) return;

    if (inputState.wasPressed(sf::Keyboard::Key::Left) ||
        inputState.wasPressed(sf::Keyboard::Key::Right) ||
        inputState.wasPressed(sf::Keyboard::Key::A) ||
        inputState.wasPressed(sf::Keyboard::Key::D) ||
        inputState.wasPressed(sf::Keyboard::Key::Up) ||
        inputState.wasPressed(sf::Keyboard::Key::Down) ||
        inputState.wasPressed(sf::Keyboard::Key::W) ||
        inputState.wasPressed(sf::Keyboard::Key::S)) {
        m_selectedIndex = (m_selectedIndex == 0) ? 1 : 0;
        SoundManager::getInstance().playSound(SoundId::BUMP);
    } else if (inputState.wasPressed(sf::Keyboard::Key::Enter) ||
               inputState.wasPressed(sf::Keyboard::Key::Space)) {
        confirmChoice(m_selectedIndex == 0 ? CharacterType::MARIO : CharacterType::LUIGI);
    }
}

void PvpCharacterSelectState::update(float dt) {
    m_animTimer += dt;

    if (m_phase == Phase::CONFIRM) {
        m_confirmTimer -= dt;
        if (m_confirmTimer <= 0.f) {
            queueMatch();
        }
    }

    const bool marioSelected = (m_selectedIndex == 0);
    const float pulse = (std::sin(m_animTimer * 6.f) + 1.f) * 0.5f;
    const sf::Color activeGold(
        static_cast<std::uint8_t>(220 + 35 * pulse),
        static_cast<std::uint8_t>(180 + 35 * pulse),
        0
    );

    if (marioSelected) {
        m_marioCard.setFillColor(CARD_COLOR_ACTIVE);
        m_marioCard.setOutlineColor(activeGold);
        m_marioCard.setOutlineThickness(2.5f);
        m_marioAvatarBox.setOutlineColor(activeGold);

        m_luigiCard.setFillColor(CARD_COLOR);
        m_luigiCard.setOutlineColor(CARD_OUTLINE);
        m_luigiCard.setOutlineThickness(1.f);
        m_luigiAvatarBox.setOutlineColor(CARD_OUTLINE);

        if (m_marioActionText) {
            m_marioActionText->setString("> CHOOSE MARIO <");
            m_marioActionText->setFillColor(GOLD_COLOR);
            m_marioActionText->setStyle(sf::Text::Bold);
            sf::FloatRect b = m_marioActionText->getLocalBounds();
            m_marioActionText->setOrigin({b.position.x + b.size.x / 2.f, 0.f});
        }
        if (m_luigiActionText) {
            m_luigiActionText->setString("CHOOSE LUIGI");
            m_luigiActionText->setFillColor(sf::Color(240, 245, 255));
            m_luigiActionText->setStyle(sf::Text::Regular);
            sf::FloatRect b = m_luigiActionText->getLocalBounds();
            m_luigiActionText->setOrigin({b.position.x + b.size.x / 2.f, 0.f});
        }
    } else {
        m_luigiCard.setFillColor(CARD_COLOR_ACTIVE);
        m_luigiCard.setOutlineColor(activeGold);
        m_luigiCard.setOutlineThickness(2.5f);
        m_luigiAvatarBox.setOutlineColor(activeGold);

        m_marioCard.setFillColor(CARD_COLOR);
        m_marioCard.setOutlineColor(CARD_OUTLINE);
        m_marioCard.setOutlineThickness(1.f);
        m_marioAvatarBox.setOutlineColor(CARD_OUTLINE);

        if (m_luigiActionText) {
            m_luigiActionText->setString("> CHOOSE LUIGI <");
            m_luigiActionText->setFillColor(GOLD_COLOR);
            m_luigiActionText->setStyle(sf::Text::Bold);
            sf::FloatRect b = m_luigiActionText->getLocalBounds();
            m_luigiActionText->setOrigin({b.position.x + b.size.x / 2.f, 0.f});
        }
        if (m_marioActionText) {
            m_marioActionText->setString("CHOOSE MARIO");
            m_marioActionText->setFillColor(sf::Color(240, 245, 255));
            m_marioActionText->setStyle(sf::Text::Regular);
            sf::FloatRect b = m_marioActionText->getLocalBounds();
            m_marioActionText->setOrigin({b.position.x + b.size.x / 2.f, 0.f});
        }
    }

    // Position badges
    if (m_phase == Phase::PLAYER_ONE) {
        const float targetX = (m_selectedIndex == 0) ? MARIO_CARD_X + 8.f : LUIGI_CARD_X + 8.f;
        m_p1BadgeBox.setPosition({targetX, CARD_Y - 9.f});
        if (m_p1BadgeText) m_p1BadgeText->setPosition({targetX + 13.f, CARD_Y - 7.f});
    } else if (m_phase == Phase::PLAYER_TWO) {
        const float p1X = (m_playerOneChoice == CharacterType::MARIO) ? MARIO_CARD_X + 8.f : LUIGI_CARD_X + 8.f;
        m_p1BadgeBox.setPosition({p1X, CARD_Y - 9.f});
        if (m_p1BadgeText) m_p1BadgeText->setPosition({p1X + 13.f, CARD_Y - 7.f});

        const float p2X = (m_selectedIndex == 0) ? MARIO_CARD_X + (CARD_WIDTH - 54.f) : LUIGI_CARD_X + (CARD_WIDTH - 54.f);
        m_p2BadgeBox.setPosition({p2X, CARD_Y - 9.f});
        if (m_p2BadgeText) m_p2BadgeText->setPosition({p2X + 13.f, CARD_Y - 7.f});
    } else if (m_phase == Phase::CONFIRM) {
        const float p1X = (m_playerOneChoice == CharacterType::MARIO) ? MARIO_CARD_X + 8.f : LUIGI_CARD_X + 8.f;
        m_p1BadgeBox.setPosition({p1X, CARD_Y - 9.f});
        if (m_p1BadgeText) m_p1BadgeText->setPosition({p1X + 13.f, CARD_Y - 7.f});

        const float p2X = (m_playerTwoChoice == CharacterType::MARIO) ? MARIO_CARD_X + (CARD_WIDTH - 54.f) : LUIGI_CARD_X + (CARD_WIDTH - 54.f);
        m_p2BadgeBox.setPosition({p2X, CARD_Y - 9.f});
        if (m_p2BadgeText) m_p2BadgeText->setPosition({p2X + 13.f, CARD_Y - 7.f});
    }

    if (m_characterTextureLoaded) {
        const int frame = static_cast<int>(m_animTimer / 0.15f) % 3;
        static const sf::IntRect bigMarioWalk[3] = {
            SpriteFrames::shared::BigMario::WALK1,
            SpriteFrames::shared::BigMario::WALK2,
            SpriteFrames::shared::BigMario::WALK3
        };
        static const sf::IntRect bigLuigiWalk[3] = {
            SpriteFrames::shared::BigLuigi::WALK1,
            SpriteFrames::shared::BigLuigi::WALK2,
            SpriteFrames::shared::BigLuigi::WALK3
        };

        if (m_marioSprite) {
            m_marioSprite->setTextureRect(bigMarioWalk[frame]);
        }
        if (m_luigiSprite) {
            m_luigiSprite->setTextureRect(bigLuigiWalk[frame]);
        }
    }
}

void PvpCharacterSelectState::render(sf::RenderTarget& target) {
    target.clear(SKY_COLOR);
    target.setView(target.getDefaultView());

    target.draw(m_panel);
    target.draw(m_marioCard);
    target.draw(m_luigiCard);
    target.draw(m_marioHeader);
    target.draw(m_luigiHeader);
    target.draw(m_marioAvatarBox);
    target.draw(m_luigiAvatarBox);

    if (m_characterTextureLoaded) {
        if (m_marioSprite) target.draw(*m_marioSprite);
        if (m_luigiSprite) target.draw(*m_luigiSprite);
    }

    // Draw player badges
    target.draw(m_p1BadgeBox);
    if (m_p1BadgeText) target.draw(*m_p1BadgeText);

    if (m_phase != Phase::PLAYER_ONE) {
        target.draw(m_p2BadgeBox);
        if (m_p2BadgeText) target.draw(*m_p2BadgeText);
    }

    if (!m_fontLoaded) return;

    if (m_titleText) target.draw(*m_titleText);
    if (m_subtitleText) target.draw(*m_subtitleText);
    if (m_marioNameText) target.draw(*m_marioNameText);
    if (m_marioRoleText) target.draw(*m_marioRoleText);
    if (m_marioStatsText) target.draw(*m_marioStatsText);
    if (m_marioActionText) target.draw(*m_marioActionText);
    if (m_luigiNameText) target.draw(*m_luigiNameText);
    if (m_luigiRoleText) target.draw(*m_luigiRoleText);
    if (m_luigiStatsText) target.draw(*m_luigiStatsText);
    if (m_luigiActionText) target.draw(*m_luigiActionText);
    if (m_hintText && m_phase != Phase::CONFIRM) target.draw(*m_hintText);
}
