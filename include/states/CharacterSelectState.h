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

    std::optional<sf::Text> m_titleText;
    std::optional<sf::Text> m_subtitleText;
    std::optional<sf::Text> m_marioDetailsText;
    std::optional<sf::Text> m_luigiDetailsText;
    std::optional<sf::Text> m_hintText;

    sf::RectangleShape m_panel;
    sf::RectangleShape m_marioCard;
    sf::RectangleShape m_luigiCard;
    sf::RectangleShape m_marioLeftBox;
    sf::RectangleShape m_marioRightBox;
    sf::RectangleShape m_luigiLeftBox;
    sf::RectangleShape m_luigiRightBox;

    std::optional<sf::Sprite> m_marioLeftSprite;
    std::optional<sf::Sprite> m_marioRightSprite;
    std::optional<sf::Sprite> m_luigiLeftSprite;
    std::optional<sf::Sprite> m_luigiRightSprite;

    std::unique_ptr<UIMenuWidget> m_menu;

    int m_selectedLevel = 1;
    float m_animTimer = 0.f;
    bool m_transitioning = false;
};
