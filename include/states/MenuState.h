/**
 * @file MenuState.h
 * @author TV1 (Dương)
 * @brief Main Menu State
 */
#pragma once
#include "states/IGameState.h"
#include <SFML/Graphics.hpp>

class MenuState : public IGameState {
public:
    // 1. Constructor / Destructor
    /**
     * @brief Constructs the MenuState with optional starting save data.
     * @param score The current score to display.
     * @param coins The current coins to display.
     * @param world The current world to display.
     * @param level The current level to display.
     * @param topScore The top score to display.
     */
    MenuState(int score = 0, int coins = 0, int world = 1, int level = 1, int topScore = 0);
    ~MenuState() override = default;

    // 2. Override methods
    /**
     * @brief Called when the state is entered. Loads textures and initializes sprites.
     */
    void onEnter() override;

    /**
     * @brief Called when the state is exited. Clean up happens automatically.
     */
    void onExit() override;

    /**
     * @brief Processes SFML events, such as pressing Enter to start the game.
     * @param event The SFML event to process.
     */
    void processEvents(const sf::Event& event) override;

    /**
     * @brief Updates the Menu logic, including the coin animation.
     * @param dt The time elapsed since the last update in seconds.
     */
    void update(float dt) override;

    /**
     * @brief Renders the Menu background, text, and sprites.
     * @param window The sf::RenderWindow to draw onto.
     */
    void render(sf::RenderWindow& window) override;

private:
    // 6. Private members
    sf::Texture m_hudTexture;
    sf::Sprite m_bgSprite;
    sf::Sprite m_cursorSprite;
    std::vector<sf::Sprite> m_dynamicTextSprites;
    sf::Font m_font;
    sf::Text m_pressToPlayText;

    int m_score;
    int m_coins;
    int m_world;
    int m_level;
    int m_topScore;

    sf::Sprite m_coinSprite;
    float m_coinAnimTimer = 0.f;
    int m_coinCurrentFrame = 0;
};
