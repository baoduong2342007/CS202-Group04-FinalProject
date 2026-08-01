/**
 * @file HUD.h
 * @author TV5 (Truyền)
 * @brief Heads-Up Display showing score and lives using pixel fonts.
 * @note Subscribes to EventBus (COIN_COLLECTED, PLAYER_DIED, PLAYER_POWER_UP)
 *       and queries Mario::getScore() directly, since EventBus::notify carries
 *       no payload. Lives are tracked locally because no lives field exists
 *       elsewhere in the codebase yet.
 */

#pragma once

// 1. Standard library
#include <optional>
#include <string>

// 2. SFML
#include <SFML/Graphics.hpp>

// 3. Project headers
#include "entities/Mario.h"
#include "patterns/EventBus.h"
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
 * @brief On-screen HUD rendering score and lives with a pixel font.
 */
class HUD : public IObserver {
public:
    // 1. Constructor / Destructor
    /// @param mario Reference to the player whose score is displayed.
    explicit HUD(const Mario& mario);
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

private:
    // 5. Private methods
    /// @brief Loads the pixel font; returns true on success.
    bool loadFont(const std::string& filepath);

    /// @brief Builds the score/lives text strings from current state.
    void refreshText();

    // 6. Private members
    const Mario& m_mario;       ///< Player reference for score queries.
    sf::Font m_font;            ///< Pixel font (openFromFile returns bool).
    std::optional<sf::Text> m_scoreText; ///< "SCORE 000000" (nullopt if no font)
    std::optional<sf::Text> m_livesText; ///< "LIVES x 3" (nullopt if no font)
    int m_lives;                ///< Lives counter tracked by the HUD.
    bool m_fontLoaded;          ///< Whether the font loaded successfully.
};
