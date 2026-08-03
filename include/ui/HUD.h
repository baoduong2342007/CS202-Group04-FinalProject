/**
 * @file HUD.h
 * @author TV5 (Truyền)
 * @brief Heads-Up Display showing score, lives, coin count, and world indicator.
 * @note Subscribes to EventBus (COIN_COLLECTED, PLAYER_DIED, PLAYER_POWER_UP)
 *       and queries Mario::getScore() directly, since EventBus::notify carries
 *       no payload. Coin count is tracked locally via COIN_COLLECTED events.
 */

#pragma once

// 1. Standard library
#include <optional>
#include <string>

// 2. SFML
#include <SFML/Graphics.hpp>

// 3. Project headers
#include "entities/Mario.h"
#include "patterns/EventType.h"
#include "patterns/IObserver.h"

// ============================================================
// PATTERN: Observer (Subscriber)
// Reason: The HUD must react to gameplay events (coin collected,
//         player death, power-up) without being coupled to the
//         entities that raise them. It queries Mario for the
//         authoritative score on each update.
// ============================================================

/**
 * @brief On-screen HUD rendering score, lives, coin count, and world indicator.
 */
class HUD : public IObserver {
public:
    // 1. Constructor / Destructor
    /// @param mario Reference to the player whose score is displayed.
    /// @param worldNumber The world number to display (e.g., 1).
    /// @param levelNumber The level number within the world (e.g., 1).
    explicit HUD(const Mario& mario, int worldNumber = 1, int levelNumber = 1);
    ~HUD() override;

    // 2. Override methods (IObserver)
    /// @brief Refreshes the displayed values when a subscribed event fires.
    void onNotify(EventType event) override;

    // 3. Public methods
    /// @brief Re-reads score/lives and updates the text objects.
    void update();

    /// @brief Draws the HUD onto the render target.
    void draw(sf::RenderTarget& target) const;

    // 4. Getters / Setters
    int getLives() const;
    void setLives(int lives);
    int getCoinCount() const;
    void setWorldLevel(int world, int level);

private:
    // 5. Private methods
    /// @brief Loads the pixel font; returns true on success.
    bool loadFont(const std::string& filepath);

    /// @brief Builds all HUD text strings from current state.
    void refreshText();

    // 6. Private members
    const Mario& m_mario;       ///< Player reference for score queries.
    sf::Font m_font;            ///< Pixel font (openFromFile returns bool).
    std::optional<sf::Text> m_scoreText;  ///< "SCORE 000000"
    std::optional<sf::Text> m_livesText;  ///< "LIVES x 3"
    std::optional<sf::Text> m_coinText;   ///< "COINS x 05"
    std::optional<sf::Text> m_worldText;  ///< "WORLD 1-1"
    int m_lives;                ///< Lives counter tracked by the HUD.
    int m_coinCount;            ///< Coin counter incremented on COIN_COLLECTED.
    int m_worldNumber;          ///< Current world number.
    int m_levelNumber;          ///< Current level number.
    bool m_fontLoaded;          ///< Whether the font loaded successfully.
};
