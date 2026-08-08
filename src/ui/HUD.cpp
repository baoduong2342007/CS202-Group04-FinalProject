/**
 * @file HUD.cpp
 * @author TV5 (Truyền)
 * @brief Implementation of HUD — score, lives, coin count, and world indicator.
 * @note sf::Font::loadFromFile returns bool (unlike sf::Texture/sf::SoundBuffer
 *       constructors in SFML 3 which throw). The return value is checked and
 *       the HUD degrades gracefully if the font file is missing.
 */

#include "ui/HUD.h"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <utility>
#include "patterns/EventBus.h"
#include <iomanip>
#include <sstream>

#ifdef DEBUG
#include <iostream>
#endif

// ============================================================
// PATTERN: Observer (Subscriber)
// Reason: HUD subscribes to gameplay events so it can refresh its
//         display without being coupled to the event publishers.
// ============================================================

namespace {
// ── Constants ────────────────────────────────────────────────
constexpr const char *FONT_PATH = "assets/fonts/mario.ttf";
// Fallback fonts (system paths) used when the pixel font is missing.
// SFML 3.0.0 has no sf::Font::getDefaultFont(), so we try common system fonts.
constexpr const char *FALLBACK_FONT_PATHS[] = {
    "C:/Windows/Fonts/arial.ttf",
    "C:/Windows/Fonts/segoeui.ttf",
    "C:/Windows/Fonts/consola.ttf",
};
constexpr unsigned int FONT_SIZE = 8;

// HUD layout positions (screen-space, drawn on default view)
constexpr float SCORE_X = 6.f;
constexpr float SCORE_Y = 3.f;
constexpr float COINS_X = 120.f;
constexpr float COINS_Y = 3.f;
constexpr float WORLD_X = 210.f;
constexpr float WORLD_Y = 3.f;
constexpr float LIVES_X = 300.f;
constexpr float LIVES_Y = 3.f;
constexpr float TIME_X = 430.f;
constexpr float TIME_Y = 3.f;
// The current logical canvas is 426x240. Keep the power indicator on a
// second compact HUD row until the display module moves to its locked 640x360
// canvas; this prevents POWER from being clipped in the current build.
constexpr float POWER_X = 4.f;
constexpr float POWER_Y = 16.f;
constexpr unsigned int SECOND = 1;

// Text padding widths
constexpr int SCORE_PAD_WIDTH = 6;
constexpr int COIN_PAD_WIDTH = 2;
} // namespace

// ── Constructor / Destructor ─────────────────────────────────

HUD::HUD(const Mario &mario, int worldNumber, int levelNumber)
    : m_mario(mario), m_worldNumber(worldNumber), m_levelNumber(levelNumber),
      m_fontLoaded(false) {
  // Load the pixel font; check the bool return value.
  m_fontLoaded = loadFont(FONT_PATH);

    // SFML 3: sf::Text requires a font at construction time, so we
    // only construct the text objects when the font is available.
    if (m_fontLoaded) {
        m_scoreText.emplace(m_font);
        m_livesText.emplace(m_font);
        m_coinText.emplace(m_font);
        m_worldText.emplace(m_font);
        m_timeText.emplace(m_font);
        m_powerText.emplace(m_font);

    m_scoreText->setCharacterSize(FONT_SIZE);
    m_scoreText->setFillColor(sf::Color::White);
    m_scoreText->setPosition({SCORE_X, SCORE_Y});

    m_livesText->setCharacterSize(FONT_SIZE);
    m_livesText->setFillColor(sf::Color::White);
    m_livesText->setPosition({LIVES_X, LIVES_Y});

    m_coinText->setCharacterSize(FONT_SIZE);
    m_coinText->setFillColor(sf::Color::Yellow);
    m_coinText->setPosition({COINS_X, COINS_Y});

        m_worldText->setCharacterSize(FONT_SIZE);
        m_worldText->setFillColor(sf::Color::White);
        m_worldText->setPosition({WORLD_X, WORLD_Y});

        m_timeText->setCharacterSize(FONT_SIZE);
        m_timeText->setFillColor(sf::Color::White);
        m_timeText->setPosition({TIME_X, TIME_Y});

        m_powerText->setCharacterSize(FONT_SIZE);
        m_powerText->setFillColor(sf::Color::White);
        m_powerText->setPosition({POWER_X, POWER_Y});
    }

    // Subscribe to gameplay events that affect the display.
    EventBus& bus = EventBus::getInstance();
    bus.subscribe(EventType::COIN_COLLECTED, this);
    bus.subscribe(EventType::PLAYER_DIED, this);
    bus.subscribe(EventType::PLAYER_LOST_LIFE, this);
    bus.subscribe(EventType::PLAYER_POWER_UP, this);
    bus.subscribe(EventType::PLAYER_STAR_COLLECTED, this);
    bus.subscribe(EventType::PLAYER_INVINCIBILITY_EXPIRED, this);
    bus.subscribe(EventType::ONE_UP_COLLECTED, this);
    bus.subscribe(EventType::GAME_PAUSED, this);
    bus.subscribe(EventType::LEVEL_COMPLETED, this);
    bus.subscribe(EventType::LEVEL_STARTED, this);

  // Render the initial values.
  refreshText();
}

HUD::~HUD() {
    // Observer contract: unsubscribe to avoid dangling pointers.
    EventBus& bus = EventBus::getInstance();
    bus.unsubscribe(EventType::COIN_COLLECTED, this);
    bus.unsubscribe(EventType::PLAYER_DIED, this);
    bus.unsubscribe(EventType::PLAYER_LOST_LIFE, this);
    bus.unsubscribe(EventType::PLAYER_POWER_UP, this);
    bus.unsubscribe(EventType::PLAYER_STAR_COLLECTED, this);
    bus.unsubscribe(EventType::PLAYER_INVINCIBILITY_EXPIRED, this);
    bus.unsubscribe(EventType::ONE_UP_COLLECTED, this);
    bus.unsubscribe(EventType::GAME_PAUSED, this);
    bus.unsubscribe(EventType::LEVEL_COMPLETED, this);
    bus.unsubscribe(EventType::LEVEL_STARTED, this);
}

// ── IObserver ────────────────────────────────────────────────

void HUD::onNotify(EventType event) {
    switch (event) {
        case EventType::COIN_COLLECTED:
            refreshText();
            break;
        case EventType::PLAYER_DIED:
        case EventType::PLAYER_LOST_LIFE:
            // Refresh the display.
            refreshText();
            break;
        case EventType::PLAYER_POWER_UP:
            // Power-up may change score; refresh to be safe.
            refreshText();
            break;
        case EventType::PLAYER_STAR_COLLECTED:
            m_starPowerActive = true;
            refreshText();
            break;
        case EventType::PLAYER_INVINCIBILITY_EXPIRED:
            m_starPowerActive = false;
            refreshText();
            break;
        case EventType::ONE_UP_COLLECTED:
            refreshText();
            break;
        case EventType::GAME_PAUSED:
            // The overlay stops PlayState updates. Skip the current frame as
            // well because the pause event is delivered during input/update.
            m_timerPausedForEvent = true;
            break;
        case EventType::LEVEL_COMPLETED:
            m_timerEnabled = false;
            break;
        case EventType::LEVEL_STARTED:
            resetTimer();
            break;
        default:
            break;
    }
}

// ── Public methods ───────────────────────────────────────────

void HUD::update() {
  // Re-read authoritative score from Mario each frame.
  refreshText();
}

void HUD::update(float dt, bool gameplayActive) {
    advanceTimer(dt, gameplayActive);
    refreshText();
}

void HUD::draw(sf::RenderTarget& target) const {
    // Only draw if the font loaded; otherwise the text is invisible.
    if (m_fontLoaded) {
        target.draw(*m_scoreText);
        target.draw(*m_livesText);
        target.draw(*m_coinText);
        target.draw(*m_worldText);
        target.draw(*m_timeText);
        target.draw(*m_powerText);
    }
}

// ── Getters / Setters ────────────────────────────────────────

int HUD::getCoinCount() const { return m_mario.getCoinCount(); }

std::string HUD::getPowerLabel() const {
    if (m_starPowerActive) {
        return "STAR";
    }

    switch (m_mario.getMarioState()) {
        case MarioState::SMALL:
            return "SMALL";
        case MarioState::SUPER:
            return "SUPER";
        case MarioState::FIRE:
        case MarioState::FIRE_SMALL:
            return "FIRE";
    }

    return "SMALL";
}

void HUD::setWorldLevel(int world, int level) {
  m_worldNumber = world;
  m_levelNumber = level;
  refreshText();
}

bool HUD::isTimeWarningActive() const {
    return m_timeRemaining <= TIME_WARNING_THRESHOLD && m_timeRemaining > 0;
}

void HUD::resetTimer(int seconds) {
    m_timeRemaining = std::max(0, seconds);
    m_timerAccumulator = 0.f;
    m_timeWarningEmitted = false;
    m_timerPausedForEvent = false;
    m_timerEnabled = true;
    refreshText();
}

void HUD::setTimeWarningCallback(std::function<void()> callback) {
    m_timeWarningCallback = std::move(callback);
}

void HUD::setTimeoutCallback(std::function<void()> callback) {
    m_timeoutCallback = std::move(callback);
}

// ── Private methods ──────────────────────────────────────────

bool HUD::loadFont(const std::string &filepath) {
  // SFML 3: sf::Font::openFromFile returns bool — check it explicitly.
  if (m_font.openFromFile(filepath)) {
    return true;
  }

  // Preferred pixel font missing — fall back to a system font so the HUD
  // still renders instead of silently disappearing.
#ifdef DEBUG
  std::cerr << "[DEBUG][HUD] Failed to load font from '" << filepath
            << "'. Trying system fallback fonts...\n";
#endif
  for (const char *fallback : FALLBACK_FONT_PATHS) {
    if (m_font.openFromFile(fallback)) {
#ifdef DEBUG
      std::cerr << "[DEBUG][HUD] Using fallback font '" << fallback << "'\n";
#endif
      return true;
    }
  }

  // No font available at all — HUD text will not be drawn.
#ifdef DEBUG
  std::cerr << "[DEBUG][HUD] All fallback fonts failed. "
            << "HUD text will not be drawn.\n";
#endif
  return false;
}

void HUD::refreshText() {
  if (!m_fontLoaded) {
    return;
  }

    // Format score as a zero-padded 6-digit string (classic Mario style).
    std::ostringstream scoreStream;
    const int displayScore = std::clamp(m_mario.getScore(), 0, 999999);
    scoreStream << "SCORE " << std::setw(SCORE_PAD_WIDTH) << std::setfill('0')
                << displayScore;
    m_scoreText->setString(scoreStream.str());

  // Format lives as "LIVES x N".
  std::ostringstream livesStream;
  livesStream << "LIVES x " << m_mario.getLives();
  m_livesText->setString(livesStream.str());

  // Format coin count with padding.
  std::ostringstream coinStream;
  coinStream << "COINS x " << std::setw(COIN_PAD_WIDTH) << std::setfill('0')
             << m_mario.getCoinCount();
  m_coinText->setString(coinStream.str());

    // Format world indicator as "WORLD W-L".
    std::ostringstream worldStream;
    worldStream << "WORLD " << m_worldNumber << "-" << m_levelNumber;
    m_worldText->setString(worldStream.str());

    std::ostringstream timeStream;
    timeStream << "TIME " << std::setw(3) << std::setfill('0')
               << m_timeRemaining;
    m_timeText->setString(timeStream.str());
    m_timeText->setFillColor(isTimeWarningActive() ? sf::Color::Red : sf::Color::White);

    m_powerText->setString("POWER " + getPowerLabel());
}

void HUD::advanceTimer(float dt, bool gameplayActive) {
    if (!gameplayActive || !m_timerEnabled || m_timerPausedForEvent ||
        m_timeRemaining <= 0 || !std::isfinite(dt) || dt <= 0.f) {
        m_timerPausedForEvent = false;
        return;
    }

    m_timerAccumulator += dt;
    while (m_timerAccumulator >= static_cast<float>(SECOND) && m_timeRemaining > 0) {
        m_timerAccumulator -= static_cast<float>(SECOND);
        --m_timeRemaining;

        if (isTimeWarningActive() && !m_timeWarningEmitted) {
            m_timeWarningEmitted = true;
            if (m_timeWarningCallback) {
                m_timeWarningCallback();
            }
        }

        if (m_timeRemaining == 0 && m_timeoutCallback) {
            m_timerEnabled = false;
            m_timeoutCallback();
        }
    }
}
