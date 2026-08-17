/**
 * @file CharacterSelectState.h
 * @author TV5 (Truyền)
 * @brief Character selection state shown between the main menu and gameplay.
 */
#pragma once

#include "entities/Mario.h"
#include "states/IGameState.h"
#include "ui/UIMenuWidget.h"

#include <SFML/Graphics.hpp>

#include <memory>
#include <optional>

/**
 * @brief Presents the playable character choices and starts a fresh game.
 *
 * The state owns the only normal menu-to-game callbacks.  Keeping the
 * character identity in each callback makes the choice explicit and avoids
 * falling back to PlayState's default constructor.
 */
class CharacterSelectState final : public IGameState {
public:
    explicit CharacterSelectState(int selectedLevel = 1);
    ~CharacterSelectState() override = default;

    void onEnter() override;
    void onExit() override;
    void processEvents(const sf::Event& event) override;
    void processInput(const InputState& inputState) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    void initBackdropPanel();
    void initCards();
    void initAvatars();
    void initTextLabels();
    void initMenu();
    void queuePlay(CharacterType characterType);

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
    std::optional<sf::Text> m_hintText;

    std::unique_ptr<UIMenuWidget> m_menu;

    int m_selectedLevel = 1;
    int m_selectedIndex = 0;
    float m_animTimer = 0.f;
    bool m_transitioning = false;
};
