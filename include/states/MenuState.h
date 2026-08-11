/**
 * @file MenuState.h
 * @author TV1 (Dương)
 * @brief Main Menu State
 */
#pragma once
#include "states/IGameState.h"
#include <SFML/Graphics.hpp>
#include <optional>

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
    void processInput(const InputState& inputState) override;

    /**
     * @brief Updates the Menu logic, including the coin animation.
     * @param dt The time elapsed since the last update in seconds.
     */
    void update(float dt) override;

    /**
     * @brief Renders the Menu background, text, and sprites.
     * @param window The sf::RenderWindow to draw onto.
     */
    void render(sf::RenderTarget& target) override;

private:
    // 6. Private members
    sf::Texture m_hudTexture;                   ///< Texture containing HUD elements
    sf::Sprite m_bgSprite;                      ///< Background sprite
    sf::Sprite m_cursorSprite;                  ///< Menu cursor sprite
    std::vector<sf::Sprite> m_dynamicTextSprites; ///< Sprites for dynamic text
    sf::Font m_font;                            ///< Font for rendering text
    bool m_fontLoaded;                          ///< True if font loaded successfully
    std::optional<sf::Text> m_pressToPlayText;  ///< Blinking "PRESS TO PLAY" text

    int m_score;                                ///< Current score
    int m_coins;                                ///< Current coins
    int m_world;                                ///< Current world
    int m_level;                                ///< Current level
    int m_topScore;                             ///< High score

    sf::Sprite m_coinSprite;                    ///< Coin animation sprite
    float m_coinAnimTimer = 0.f;                ///< Timer for coin animation
    int m_coinCurrentFrame = 0;                 ///< Current frame of coin animation
    float m_blinkTimer = 0.f;                   ///< Timer for text blinking
    bool m_showPressToPlay = true;              ///< Toggle for blinking text
    bool m_transitioning = false;               ///< True if transitioning to play state
};
