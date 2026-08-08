/**
 * @file HUD.h
 * @author TV5 (Truyền)
 * @brief Heads-Up Display showing score, lives, coin count, and world indicator.
 * @note Subscribes to EventBus for refresh triggers, while Mario remains the
 *       authoritative source for score, lives, and coin count.
 */

#pragma once

// 1. Standard library
#include <functional>
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

    /// @brief Advances the level timer and refreshes all HUD values.
    /// @param dt Seconds elapsed since the previous frame.
    /// @param gameplayActive False while paused, transitioning, or otherwise
    ///        frozen; the timer does not advance in that case.
    void update(float dt, bool gameplayActive);

    /// @brief Draws the HUD onto the render target.
    void draw(sf::RenderTarget& target) const;

    /**
     * @brief Retrieves the current coin count.
     * @return int The number of coins currently held.
     */
    int getCoinCount() const;
    /// Return the label rendered by the power indicator.
    std::string getPowerLabel() const;

    /**
     * @brief Updates the world and level number displayed on the HUD.
     * @param world The world number (e.g., 1 for World 1-1).
     * @param level The level number (e.g., 1 for World 1-1).
     */
    void setWorldLevel(int world, int level);
    int getTimeRemaining() const { return m_timeRemaining; }
    bool isTimeWarningActive() const;
    bool hasTimedOut() const { return m_timeRemaining == 0; }

    /// Reset the countdown for a newly loaded level.
    void resetTimer(int seconds = DEFAULT_LEVEL_TIME);

    /// Optional hooks let the state/gameplay owner connect warning and
    /// timeout behavior without coupling HUD to SoundManager or Mario death.
    void setTimeWarningCallback(std::function<void()> callback);
    void setTimeoutCallback(std::function<void()> callback);

    static constexpr int DEFAULT_LEVEL_TIME = 400;
    static constexpr int TIME_WARNING_THRESHOLD = 100;

private:
    // 5. Private methods
    /// @brief Loads the pixel font; returns true on success.
    bool loadFont(const std::string& filepath);

    /// @brief Builds all HUD text strings from current state.
    void refreshText();
    void advanceTimer(float dt, bool gameplayActive);

    // 6. Private members
    const Mario& m_mario;       ///< Player reference for score queries.
    sf::Font m_font;            ///< Pixel font (openFromFile returns bool).
    std::optional<sf::Text> m_scoreText;  ///< "SCORE 000000"
    std::optional<sf::Text> m_livesText;  ///< "LIVES x 3"
    std::optional<sf::Text> m_coinText;   ///< "COINS x 05"
    std::optional<sf::Text> m_worldText;  ///< "WORLD 1-1"
    std::optional<sf::Text> m_timeText;   ///< "TIME 400"
    std::optional<sf::Text> m_powerText;  ///< "POWER FIRE" / "POWER STAR"
    int m_worldNumber;          ///< Current world number.
    int m_levelNumber;          ///< Current level number.
    bool m_fontLoaded;          ///< Whether the font loaded successfully.
    bool m_starPowerActive = false;

    int m_timeRemaining = DEFAULT_LEVEL_TIME;
    float m_timerAccumulator = 0.f;
    bool m_timeWarningEmitted = false;
    bool m_timerPausedForEvent = false;
    bool m_timerEnabled = true;
    std::function<void()> m_timeWarningCallback;
    std::function<void()> m_timeoutCallback;
};
