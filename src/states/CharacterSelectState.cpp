/**
 * @file CharacterSelectState.cpp
 * @author TV5 (Truyền)
 * @brief Character selection state implementation.
 */
#include "states/CharacterSelectState.h"

#include "core/DisplayConfig.h"
#include "core/GameManager.h"
#include "core/SoundManager.h"
#include "core/SpriteFrames.h"
#include "patterns/InputState.h"
#include "states/LevelSelectState.h"
#include "states/PlayState.h"
#include "ui/UILayoutHelper.h"

#include <cmath>

namespace {
constexpr const char* FONT_PATH = "assets/fonts/mario.ttf";
constexpr const char* CHARACTER_TEXTURE_PATH =
    "assets/textures/mario/MarioLuigi.png";

constexpr unsigned int TITLE_FONT_SIZE = 24;
constexpr unsigned int SUBTITLE_FONT_SIZE = 12;
constexpr unsigned int DETAIL_FONT_SIZE = 11;
constexpr unsigned int HINT_FONT_SIZE = 11;
constexpr unsigned int MENU_FONT_SIZE = 14;

constexpr float TITLE_Y = 45.f;
constexpr float SUBTITLE_Y = 78.f;
constexpr float MENU_CENTER_Y = 178.f;
constexpr float MARIO_DETAILS_Y = 159.f;
constexpr float LUIGI_DETAILS_Y = 240.f;
constexpr float HINT_Y = 322.f;

constexpr float CARD_X = 70.f;
constexpr float CARD_WIDTH = 500.f;
constexpr float CARD_HEIGHT = 70.f;

constexpr float AVATAR_BOX_SIZE = 56.f;
constexpr float LEFT_BOX_X = 78.f;
constexpr float RIGHT_BOX_X = 506.f;
constexpr float MARIO_BOX_Y = 119.f;
constexpr float LUIGI_BOX_Y = 200.f;

constexpr float BIG_HERO_SCALE = 1.55f;
constexpr float SMALL_HERO_SCALE = 2.0f;

const sf::Color SKY_COLOR(28, 42, 82);
const sf::Color PANEL_COLOR(17, 24, 52);
const sf::Color CARD_COLOR(33, 47, 88);
const sf::Color CARD_OUTLINE(92, 148, 252);
const sf::Color GOLD_COLOR(255, 215, 0);
const sf::Color MUTED_GOLD_COLOR(255, 231, 128);
const sf::Color BODY_COLOR(235, 240, 255);
const sf::Color AVATAR_BOX_BG(14, 20, 44);
const sf::Color AVATAR_BOX_OUTLINE(65, 95, 155);
const sf::Color AVATAR_BOX_OUTLINE_ACTIVE(255, 215, 0);
} // namespace

CharacterSelectState::CharacterSelectState(int selectedLevel)
    : m_selectedLevel(selectedLevel) {}

void CharacterSelectState::initBackdropPanel() {
    m_panel.setSize({540.f, 324.f});
    m_panel.setPosition({50.f, 18.f});
    m_panel.setFillColor(PANEL_COLOR);
    m_panel.setOutlineColor(GOLD_COLOR);
    m_panel.setOutlineThickness(3.f);
}

void CharacterSelectState::initCards() {
    m_marioCard.setSize({CARD_WIDTH, CARD_HEIGHT});
    m_marioCard.setPosition({CARD_X, 112.f});
    m_marioCard.setFillColor(CARD_COLOR);
    m_marioCard.setOutlineColor(GOLD_COLOR);
    m_marioCard.setOutlineThickness(2.f);

    m_luigiCard.setSize({CARD_WIDTH, CARD_HEIGHT});
    m_luigiCard.setPosition({CARD_X, 193.f});
    m_luigiCard.setFillColor(CARD_COLOR);
    m_luigiCard.setOutlineColor(CARD_OUTLINE);
    m_luigiCard.setOutlineThickness(1.f);

    m_marioLeftBox.setSize({AVATAR_BOX_SIZE, AVATAR_BOX_SIZE});
    m_marioLeftBox.setPosition({LEFT_BOX_X, MARIO_BOX_Y});
    m_marioLeftBox.setFillColor(AVATAR_BOX_BG);
    m_marioLeftBox.setOutlineColor(AVATAR_BOX_OUTLINE_ACTIVE);
    m_marioLeftBox.setOutlineThickness(1.5f);

    m_marioRightBox.setSize({AVATAR_BOX_SIZE, AVATAR_BOX_SIZE});
    m_marioRightBox.setPosition({RIGHT_BOX_X, MARIO_BOX_Y});
    m_marioRightBox.setFillColor(AVATAR_BOX_BG);
    m_marioRightBox.setOutlineColor(AVATAR_BOX_OUTLINE_ACTIVE);
    m_marioRightBox.setOutlineThickness(1.5f);

    m_luigiLeftBox.setSize({AVATAR_BOX_SIZE, AVATAR_BOX_SIZE});
    m_luigiLeftBox.setPosition({LEFT_BOX_X, LUIGI_BOX_Y});
    m_luigiLeftBox.setFillColor(AVATAR_BOX_BG);
    m_luigiLeftBox.setOutlineColor(AVATAR_BOX_OUTLINE);
    m_luigiLeftBox.setOutlineThickness(1.f);

    m_luigiRightBox.setSize({AVATAR_BOX_SIZE, AVATAR_BOX_SIZE});
    m_luigiRightBox.setPosition({RIGHT_BOX_X, LUIGI_BOX_Y});
    m_luigiRightBox.setFillColor(AVATAR_BOX_BG);
    m_luigiRightBox.setOutlineColor(AVATAR_BOX_OUTLINE);
    m_luigiRightBox.setOutlineThickness(1.f);
}

void CharacterSelectState::initAvatars() {
    m_characterTextureLoaded = false;
    try {
        m_characterTexture.emplace(CHARACTER_TEXTURE_PATH);
        m_characterTextureLoaded = true;
    } catch (const std::exception&) {
        m_characterTexture.reset();
        m_characterTextureLoaded = false;
    }

    if (m_characterTextureLoaded && m_characterTexture) {
        // Mario Left Avatar: Big Mario
        m_marioLeftSprite.emplace(*m_characterTexture);
        m_marioLeftSprite->setTextureRect(SpriteFrames::shared::BigMario::WALK1);
        m_marioLeftSprite->setScale({BIG_HERO_SCALE, BIG_HERO_SCALE});
        m_marioLeftSprite->setPosition({
            LEFT_BOX_X + (AVATAR_BOX_SIZE - 16.f * BIG_HERO_SCALE) / 2.f,
            MARIO_BOX_Y + (AVATAR_BOX_SIZE - 32.f * BIG_HERO_SCALE) / 2.f
        });

        // Mario Right Avatar: Small Mario
        m_marioRightSprite.emplace(*m_characterTexture);
        m_marioRightSprite->setTextureRect(SpriteFrames::shared::SmallMario::WALK1);
        m_marioRightSprite->setScale({SMALL_HERO_SCALE, SMALL_HERO_SCALE});
        m_marioRightSprite->setPosition({
            RIGHT_BOX_X + (AVATAR_BOX_SIZE - 16.f * SMALL_HERO_SCALE) / 2.f,
            MARIO_BOX_Y + (AVATAR_BOX_SIZE - 16.f * SMALL_HERO_SCALE) / 2.f
        });

        // Luigi Left Avatar: Big Luigi
        m_luigiLeftSprite.emplace(*m_characterTexture);
        m_luigiLeftSprite->setTextureRect(SpriteFrames::shared::BigLuigi::WALK1);
        m_luigiLeftSprite->setScale({BIG_HERO_SCALE, BIG_HERO_SCALE});
        m_luigiLeftSprite->setPosition({
            LEFT_BOX_X + (AVATAR_BOX_SIZE - 16.f * BIG_HERO_SCALE) / 2.f,
            LUIGI_BOX_Y + (AVATAR_BOX_SIZE - 32.f * BIG_HERO_SCALE) / 2.f
        });

        // Luigi Right Avatar: Small Luigi
        m_luigiRightSprite.emplace(*m_characterTexture);
        m_luigiRightSprite->setTextureRect(SpriteFrames::shared::SmallLuigi::JUMP);
        m_luigiRightSprite->setScale({SMALL_HERO_SCALE, SMALL_HERO_SCALE});
        m_luigiRightSprite->setPosition({
            RIGHT_BOX_X + (AVATAR_BOX_SIZE - 16.f * SMALL_HERO_SCALE) / 2.f,
            LUIGI_BOX_Y + (AVATAR_BOX_SIZE - 16.f * SMALL_HERO_SCALE) / 2.f
        });
    }
}

void CharacterSelectState::initTextLabels() {
    if (!m_fontLoaded) {
        return;
    }

    m_titleText.emplace(m_font, "CHOOSE YOUR HERO", TITLE_FONT_SIZE);
    m_titleText->setFillColor(GOLD_COLOR);
    m_titleText->setOutlineColor(sf::Color::Black);
    m_titleText->setOutlineThickness(1.f);
    UILayoutHelper::setPosition(*m_titleText, UIAnchor::TopCenter,
                                {0.f, TITLE_Y});

    m_subtitleText.emplace(
        m_font, "FASTER RUN OR HIGHER JUMP?\nCHOOSE YOUR STYLE",
        SUBTITLE_FONT_SIZE);
    m_subtitleText->setFillColor(BODY_COLOR);
    UILayoutHelper::setPosition(*m_subtitleText, UIAnchor::TopCenter,
                                {0.f, SUBTITLE_Y});

    m_marioDetailsText.emplace(m_font, "Mario: faster run, standard jump",
                                DETAIL_FONT_SIZE);
    m_marioDetailsText->setFillColor(MUTED_GOLD_COLOR);
    UILayoutHelper::setPosition(*m_marioDetailsText, UIAnchor::TopCenter,
                                {0.f, MARIO_DETAILS_Y});

    m_luigiDetailsText.emplace(m_font, "Luigi: higher jump, slower run",
                                DETAIL_FONT_SIZE);
    m_luigiDetailsText->setFillColor(MUTED_GOLD_COLOR);
    UILayoutHelper::setPosition(*m_luigiDetailsText, UIAnchor::TopCenter,
                                {0.f, LUIGI_DETAILS_Y});

    m_hintText.emplace(m_font,
                       "ARROWS / W S: CHOOSE   ENTER / CLICK: SELECT",
                       HINT_FONT_SIZE);
    m_hintText->setFillColor(BODY_COLOR);
    UILayoutHelper::setPosition(*m_hintText, UIAnchor::TopCenter,
                                {0.f, HINT_Y});
}

void CharacterSelectState::initMenu() {
    if (!m_fontLoaded) {
        return;
    }

    m_menu = std::make_unique<UIMenuWidget>(m_font);
    m_menu->addItem("MARIO - FASTER RUNNER", [this]() {
        queuePlay(CharacterType::MARIO);
    }, MENU_FONT_SIZE);
    m_menu->addItem("LUIGI - HIGHER JUMPER", [this]() {
        queuePlay(CharacterType::LUIGI);
    }, MENU_FONT_SIZE);
    m_menu->setSpacing(64.f);
    m_menu->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, MENU_CENTER_Y},
                        UIAnchor::Center);
}

void CharacterSelectState::onEnter() {
    m_transitioning = false;
    m_animTimer = 0.f;
    m_menu.reset();
    m_titleText.reset();
    m_subtitleText.reset();
    m_marioDetailsText.reset();
    m_luigiDetailsText.reset();
    m_hintText.reset();

    m_marioLeftSprite.reset();
    m_marioRightSprite.reset();
    m_luigiLeftSprite.reset();
    m_luigiRightSprite.reset();

    initBackdropPanel();
    initCards();
    initAvatars();

    m_fontLoaded = m_font.openFromFile(FONT_PATH);
    initTextLabels();
    initMenu();
}

void CharacterSelectState::onExit() {}

void CharacterSelectState::queuePlay(CharacterType characterType) {
    if (m_transitioning) {
        return;
    }

    m_transitioning = true;
    GameManager::getInstance().changeState(
        std::make_unique<PlayState>(m_selectedLevel, characterType));
}

void CharacterSelectState::processEvents(const sf::Event& event) {
    if (!m_transitioning && m_menu) {
        m_menu->processEvents(event);
    }
}

void CharacterSelectState::processInput(const InputState& inputState) {
    if (!m_transitioning) {
        if (inputState.wasPressed(sf::Keyboard::Key::Escape)) {
            m_transitioning = true;
            SoundManager::getInstance().playSound("powerdown");
            GameManager::getInstance().changeState(
                std::make_unique<LevelSelectState>());
            return;
        }
        if (m_menu) {
            m_menu->processInput(inputState);
        }
    }
}

void CharacterSelectState::update(float dt) {
    m_animTimer += dt;

    if (m_menu) {
        m_menu->update(dt);
    }

    const int selectedIndex = m_menu ? m_menu->getSelectedIndex() : 0;
    const bool marioSelected = (selectedIndex == 0);

    if (marioSelected) {
        m_marioCard.setOutlineColor(GOLD_COLOR);
        m_marioCard.setOutlineThickness(2.f);
        m_marioLeftBox.setOutlineColor(AVATAR_BOX_OUTLINE_ACTIVE);
        m_marioLeftBox.setOutlineThickness(1.5f);
        m_marioRightBox.setOutlineColor(AVATAR_BOX_OUTLINE_ACTIVE);
        m_marioRightBox.setOutlineThickness(1.5f);

        m_luigiCard.setOutlineColor(CARD_OUTLINE);
        m_luigiCard.setOutlineThickness(1.f);
        m_luigiLeftBox.setOutlineColor(AVATAR_BOX_OUTLINE);
        m_luigiLeftBox.setOutlineThickness(1.f);
        m_luigiRightBox.setOutlineColor(AVATAR_BOX_OUTLINE);
        m_luigiRightBox.setOutlineThickness(1.f);
    } else {
        m_luigiCard.setOutlineColor(GOLD_COLOR);
        m_luigiCard.setOutlineThickness(2.f);
        m_luigiLeftBox.setOutlineColor(AVATAR_BOX_OUTLINE_ACTIVE);
        m_luigiLeftBox.setOutlineThickness(1.5f);
        m_luigiRightBox.setOutlineColor(AVATAR_BOX_OUTLINE_ACTIVE);
        m_luigiRightBox.setOutlineThickness(1.5f);

        m_marioCard.setOutlineColor(CARD_OUTLINE);
        m_marioCard.setOutlineThickness(1.f);
        m_marioLeftBox.setOutlineColor(AVATAR_BOX_OUTLINE);
        m_marioLeftBox.setOutlineThickness(1.f);
        m_marioRightBox.setOutlineColor(AVATAR_BOX_OUTLINE);
        m_marioRightBox.setOutlineThickness(1.f);
    }

    if (m_characterTextureLoaded) {
        const int marioFrame = static_cast<int>(m_animTimer / 0.12f) % 3;
        const int luigiFrame = static_cast<int>(m_animTimer / 0.15f) % 3;

        static const sf::IntRect bigMarioWalk[3] = {
            SpriteFrames::shared::BigMario::WALK1,
            SpriteFrames::shared::BigMario::WALK2,
            SpriteFrames::shared::BigMario::WALK3
        };
        static const sf::IntRect smallMarioWalk[3] = {
            SpriteFrames::shared::SmallMario::WALK1,
            SpriteFrames::shared::SmallMario::WALK2,
            SpriteFrames::shared::SmallMario::WALK3
        };
        static const sf::IntRect bigLuigiWalk[3] = {
            SpriteFrames::shared::BigLuigi::WALK1,
            SpriteFrames::shared::BigLuigi::WALK2,
            SpriteFrames::shared::BigLuigi::WALK3
        };

        if (m_marioLeftSprite) {
            m_marioLeftSprite->setTextureRect(bigMarioWalk[marioFrame]);
        }
        if (m_marioRightSprite) {
            m_marioRightSprite->setTextureRect(smallMarioWalk[marioFrame]);
        }
        if (m_luigiLeftSprite) {
            m_luigiLeftSprite->setTextureRect(bigLuigiWalk[luigiFrame]);
        }
        if (m_luigiRightSprite) {
            m_luigiRightSprite->setTextureRect(
                SpriteFrames::shared::SmallLuigi::JUMP);
            const float bounceY = std::sin(m_animTimer * 5.f) * 2.5f;
            const float baseY =
                LUIGI_BOX_Y +
                (AVATAR_BOX_SIZE - 16.f * SMALL_HERO_SCALE) / 2.f;
            m_luigiRightSprite->setPosition({
                RIGHT_BOX_X +
                    (AVATAR_BOX_SIZE - 16.f * SMALL_HERO_SCALE) / 2.f,
                baseY + bounceY
            });
        }
    }
}

void CharacterSelectState::render(sf::RenderTarget& target) {
    target.clear(SKY_COLOR);
    target.setView(target.getDefaultView());

    target.draw(m_panel);
    target.draw(m_marioCard);
    target.draw(m_luigiCard);

    target.draw(m_marioLeftBox);
    target.draw(m_marioRightBox);
    target.draw(m_luigiLeftBox);
    target.draw(m_luigiRightBox);

    if (m_characterTextureLoaded) {
        if (m_marioLeftSprite) {
            target.draw(*m_marioLeftSprite);
        }
        if (m_marioRightSprite) {
            target.draw(*m_marioRightSprite);
        }
        if (m_luigiLeftSprite) {
            target.draw(*m_luigiLeftSprite);
        }
        if (m_luigiRightSprite) {
            target.draw(*m_luigiRightSprite);
        }
    }

    if (!m_fontLoaded) {
        return;
    }

    if (m_titleText) {
        target.draw(*m_titleText);
    }
    if (m_subtitleText) {
        target.draw(*m_subtitleText);
    }
    if (m_marioDetailsText) {
        target.draw(*m_marioDetailsText);
    }
    if (m_luigiDetailsText) {
        target.draw(*m_luigiDetailsText);
    }
    if (m_hintText) {
        target.draw(*m_hintText);
    }
    if (m_menu) {
        m_menu->draw(target);
    }
}
