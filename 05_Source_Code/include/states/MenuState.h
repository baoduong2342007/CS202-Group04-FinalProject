/**
 * @file MenuState.h
 * @author TV1 (Duong) & TV2 (Nhat)
 * @brief Polished, crisp retro-modern Title / Main Menu state
 */
#pragma once
#include "states/IGameState.h"
#include <SFML/Graphics.hpp>
#include <optional>
#include <memory>
#include <vector>
#include "ui/UIMenuWidget.h"

class MenuState : public IGameState {
public:
    // 1. Constructor / Destructor
    MenuState(int score = 0, int coins = 0, int world = 1, int level = 1, int topScore = 0);
    ~MenuState() override = default;

    // 2. Override methods
    void onEnter() override;
    void onExit() override;
    void processEvents(const sf::Event& event) override;
    void processInput(const InputState& inputState) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    void initTextures();
    void initScenery();
    void initTitleBanner();
    void initCredits();
    void initMenu();

    // 6. Private members
    sf::Texture m_tilesetTexture;
    sf::Texture m_characterTexture;
    sf::Texture m_itemsTexture;
    sf::Texture m_enemiesTexture;

    bool m_texturesLoaded = false;
    sf::Font m_font;
    bool m_fontLoaded = false;

    std::unique_ptr<UIMenuWidget> m_menu;

    // Title and Credits Text
    std::optional<sf::Text> m_titleShadowText;
    std::optional<sf::Text> m_titleMainText;
    std::optional<sf::Text> m_courseText;
    std::optional<sf::Text> m_groupText;
    std::optional<sf::Text> m_promptText;
    std::optional<sf::Text> m_copyrightText;

    // Decorative sprites
    std::vector<sf::Sprite> m_scenerySprites;
    std::optional<sf::Sprite> m_marioSprite;
    std::optional<sf::Sprite> m_luigiSprite;
    std::optional<sf::Sprite> m_goombaSprite;
    std::optional<sf::Sprite> m_cursorMushroomSprite;

    int m_score;
    int m_coins;
    int m_world;
    int m_level;
    int m_topScore;

    float m_animTimer = 0.f;
    float m_blinkTimer = 0.f;
    bool m_showPrompt = true;
    bool m_transitioning = false;
};
