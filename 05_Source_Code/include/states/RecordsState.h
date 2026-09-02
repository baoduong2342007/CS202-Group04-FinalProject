/**
 * @file RecordsState.h
 * @author Group 04
 * @brief Hall of Fame / Game Records & Match History State with screenshot capabilities.
 */

#pragma once

#include "states/IGameState.h"
#include "core/SaveManager.h"

#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class RecordsState final : public IGameState {
public:
    RecordsState();
    ~RecordsState() override = default;

    void onEnter() override;
    void onExit() override;
    void processEvents(const sf::Event& event) override;
    void processInput(const InputState& inputState) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    struct StageRecordCard {
        int levelNumber;
        sf::RectangleShape outerCard;
        sf::RectangleShape headerBanner;
        std::optional<sf::Text> titleText;
        std::optional<sf::Text> statusText;
        std::optional<sf::Text> bestScoreText;
    };

    struct MatchRow {
        std::optional<sf::Text> stageText;
        std::optional<sf::Text> modeText;
        std::optional<sf::Text> heroText;
        std::optional<sf::Text> resultText;
        std::optional<sf::Text> scoreText;
        std::optional<sf::Text> coinsText;
        std::optional<sf::Text> dateText;
        sf::RectangleShape rowBg;
    };

    void initBackdropPanel();
    void initTextLabels();
    void initStageCards();
    void updateMatchRows();

    sf::Font m_font;
    bool m_fontLoaded = false;

    sf::RectangleShape m_panel;
    std::optional<sf::Text> m_titleText;
    std::optional<sf::Text> m_subtitleText;
    std::optional<sf::Text> m_section1Title;
    std::optional<sf::Text> m_section2Title;
    std::optional<sf::Text> m_tableHeader;
    std::optional<sf::Text> m_emptyText;
    std::optional<sf::Text> m_hintText;
    std::optional<sf::Text> m_notificationText;

    std::vector<StageRecordCard> m_stageCards;
    std::vector<MatchRow> m_matchRows;

    int m_scrollOffset = 0;
    float m_animTimer = 0.f;
    float m_notificationTimer = 0.f;
    bool m_transitioning = false;
    bool m_confirmingReset = false;
};
