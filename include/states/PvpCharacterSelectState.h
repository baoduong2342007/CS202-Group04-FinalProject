/**
 * @file PvpCharacterSelectState.h
 * @author TV5 (Truyen)
 * @brief Sequential two-player character selection for the PvP duel mode.
 * @note Player one picks first, then player two (mirror picks are allowed);
 *       both fighters keep their authentic campaign stats.
 */

#pragma once

#include "entities/Mario.h"
#include "states/IGameState.h"
#include "ui/UIMenuWidget.h"

#include <SFML/Graphics.hpp>

#include <memory>
#include <optional>

class PvpCharacterSelectState final : public IGameState {
public:
    // 1. Constructor / Destructor
    PvpCharacterSelectState() = default;
    ~PvpCharacterSelectState() override = default;

    // 2. Override methods
    void onEnter() override;
    void onExit() override;
    void processEvents(const sf::Event& event) override;
    void processInput(const InputState& inputState) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

    // 3. Introspection (tests)
    /// Which fighter is currently picking: 0 = player one, 1 = player two,
    /// 2 = confirm splash before the match starts.
    int getCurrentPicker() const { return static_cast<int>(m_phase); }
    CharacterType getPlayerOneChoice() const { return m_playerOneChoice; }
    CharacterType getPlayerTwoChoice() const { return m_playerTwoChoice; }

private:
    enum class Phase {
        PLAYER_ONE = 0,
        PLAYER_TWO = 1,
        CONFIRM = 2
    };

    // 5. Private methods
    void initBackdropPanel();
    void initCards();
    void initAvatars();
    void initTextLabels();
    void initMenu();
    void rebuildMenuForPhase();
    void confirmChoice(CharacterType characterType);
    void queueMatch();

    // 6. Private members
    sf::Font m_font;
    bool m_fontLoaded = false;

    std::optional<sf::Texture> m_characterTexture;
    bool m_characterTextureLoaded = false;

    sf::RectangleShape m_panel;
    sf::RectangleShape m_marioCard;
    sf::RectangleShape m_luigiCard;
    sf::RectangleShape m_marioHeader;
    sf::RectangleShape m_luigiHeader;
    sf::RectangleShape m_marioAvatarBox;
    sf::RectangleShape m_luigiAvatarBox;
    sf::RectangleShape m_p1BadgeBox;
    sf::RectangleShape m_p2BadgeBox;

    std::optional<sf::Sprite> m_marioSprite;
    std::optional<sf::Sprite> m_luigiSprite;

    std::optional<sf::Text> m_titleText;
    std::optional<sf::Text> m_subtitleText;
    std::optional<sf::Text> m_marioNameText;
    std::optional<sf::Text> m_luigiNameText;
    std::optional<sf::Text> m_marioRoleText;
    std::optional<sf::Text> m_luigiRoleText;
    std::optional<sf::Text> m_marioStatsText;
    std::optional<sf::Text> m_luigiStatsText;
    std::optional<sf::Text> m_marioActionText;
    std::optional<sf::Text> m_luigiActionText;
    std::optional<sf::Text> m_p1BadgeText;
    std::optional<sf::Text> m_p2BadgeText;
    std::optional<sf::Text> m_hintText;

    std::unique_ptr<UIMenuWidget> m_menu;

    Phase m_phase = Phase::PLAYER_ONE;
    int m_selectedIndex = 0;
    CharacterType m_playerOneChoice = CharacterType::MARIO;
    CharacterType m_playerTwoChoice = CharacterType::LUIGI;
    float m_animTimer = 0.f;
    float m_confirmTimer = 0.f;
    bool m_transitioning = false;
};
