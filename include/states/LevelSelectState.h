/**
 * @file LevelSelectState.h
 * @author TV1 (Dương)
 * @brief Clean, uncluttered Level / Stage selection state with rich stage preview images.
 */

#pragma once

#include "states/IGameState.h"
#include "core/LevelCatalog.h"

#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include <vector>

/**
 * @brief Presents the 4 game worlds with real stage snapshot previews.
 *
 * Designed following "Less, but better" principles — clutter-free, focused cards.
 */
class LevelSelectState final : public IGameState {
public:
    LevelSelectState();
    ~LevelSelectState() override = default;

    void onEnter() override;
    void onExit() override;
    void processEvents(const sf::Event& event) override;
    void processInput(const InputState& inputState) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    struct StageCard {
        int levelNumber;
        sf::RectangleShape outerCard;
        sf::RectangleShape headerBanner;
        sf::RectangleShape previewBox;
        std::optional<sf::Text> titleText;
        std::optional<sf::Text> themeText;
        std::optional<sf::Text> actionText;
        std::optional<sf::Sprite> previewSprite;
    };

    void confirmSelection(int levelNumber);
    void selectCard(int index);

    sf::Font m_font;
    bool m_fontLoaded = false;

    std::vector<sf::Texture> m_stageTextures;
    bool m_texturesLoaded = false;

    sf::RectangleShape m_panel;
    std::optional<sf::Text> m_titleText;
    std::optional<sf::Text> m_subtitleText;
    std::optional<sf::Text> m_hintText;

    std::vector<StageCard> m_cards;
    int m_selectedIndex = 0;
    float m_animTimer = 0.f;
    bool m_transitioning = false;
};
