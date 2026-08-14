/**
 * @file CharacterSelectState.cpp
 * @brief Character selection state implementation.
 */
#include "states/CharacterSelectState.h"

#include "core/DisplayConfig.h"
#include "core/GameManager.h"
#include "patterns/InputState.h"
#include "states/PlayState.h"
#include "ui/UILayoutHelper.h"

namespace {
constexpr const char* FONT_PATH = "assets/fonts/mario.ttf";

constexpr unsigned int TITLE_FONT_SIZE = 24;
constexpr unsigned int SUBTITLE_FONT_SIZE = 12;
constexpr unsigned int DETAIL_FONT_SIZE = 11;
constexpr unsigned int HINT_FONT_SIZE = 11;

constexpr float TITLE_Y = 45.f;
constexpr float SUBTITLE_Y = 78.f;
constexpr float MENU_CENTER_Y = 178.f;
constexpr float MARIO_DETAILS_Y = 159.f;
constexpr float LUIGI_DETAILS_Y = 240.f;
constexpr float HINT_Y = 322.f;

const sf::Color SKY_COLOR(28, 42, 82);
const sf::Color PANEL_COLOR(17, 24, 52);
const sf::Color CARD_COLOR(33, 47, 88);
const sf::Color CARD_OUTLINE(92, 148, 252);
const sf::Color GOLD_COLOR(255, 215, 0);
const sf::Color MUTED_GOLD_COLOR(255, 231, 128);
const sf::Color BODY_COLOR(235, 240, 255);
}

void CharacterSelectState::onEnter() {
    m_transitioning = false;
    m_menu.reset();
    m_titleText.reset();
    m_subtitleText.reset();
    m_marioDetailsText.reset();
    m_luigiDetailsText.reset();
    m_hintText.reset();

    m_panel.setSize({540.f, 324.f});
    m_panel.setPosition({50.f, 18.f});
    m_panel.setFillColor(PANEL_COLOR);
    m_panel.setOutlineColor(GOLD_COLOR);
    m_panel.setOutlineThickness(3.f);

    m_marioCard.setSize({480.f, 70.f});
    m_marioCard.setPosition({80.f, 112.f});
    m_marioCard.setFillColor(CARD_COLOR);
    m_marioCard.setOutlineColor(CARD_OUTLINE);
    m_marioCard.setOutlineThickness(1.f);

    m_luigiCard.setSize({480.f, 70.f});
    m_luigiCard.setPosition({80.f, 193.f});
    m_luigiCard.setFillColor(CARD_COLOR);
    m_luigiCard.setOutlineColor(CARD_OUTLINE);
    m_luigiCard.setOutlineThickness(1.f);

    m_fontLoaded = m_font.openFromFile(FONT_PATH);
    if (!m_fontLoaded) {
        // Keep the state safe and drawable if the packaged font is missing.
        // There is no interactive menu without a font, so no transition can
        // be queued accidentally from an uninitialized widget.
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

    m_menu = std::make_unique<UIMenuWidget>(m_font);
    m_menu->addItem("MARIO - FASTER RUNNER", [this]() {
        queuePlay(CharacterType::MARIO);
    });
    m_menu->addItem("LUIGI - HIGHER JUMPER", [this]() {
        queuePlay(CharacterType::LUIGI);
    });
    m_menu->setSpacing(64.f);
    m_menu->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, MENU_CENTER_Y},
                        UIAnchor::Center);
}

void CharacterSelectState::onExit() {}

void CharacterSelectState::queuePlay(CharacterType characterType) {
    if (m_transitioning) {
        return;
    }

    m_transitioning = true;
    GameManager::getInstance().changeState(
        std::make_unique<PlayState>(characterType));
}

void CharacterSelectState::processEvents(const sf::Event& event) {
    if (!m_transitioning && m_menu) {
        m_menu->processEvents(event);
    }
}

void CharacterSelectState::processInput(const InputState& inputState) {
    if (!m_transitioning && m_menu) {
        m_menu->processInput(inputState);
    }
}

void CharacterSelectState::update(float dt) {
    if (m_menu) {
        m_menu->update(dt);
    }
}

void CharacterSelectState::render(sf::RenderTarget& target) {
    target.clear(SKY_COLOR);
    target.setView(target.getDefaultView());

    target.draw(m_panel);
    target.draw(m_marioCard);
    target.draw(m_luigiCard);

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
