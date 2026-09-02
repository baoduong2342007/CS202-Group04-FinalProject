/**
 * @file HUD.cpp
 * @author TV5 (Truyen)
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
constexpr unsigned int FONT_SIZE = 10;
constexpr unsigned int POWER_FONT_SIZE = 8;

// HUD layout positions (screen-space across 640x360 canvas)
constexpr float SCORE_X = 16.f;
constexpr float SCORE_Y = 6.f;
constexpr float COINS_X = 150.f;
constexpr float COINS_Y = 6.f;
constexpr float WORLD_X = 275.f;
constexpr float WORLD_Y = 6.f;
constexpr float TIME_X = 405.f;
constexpr float TIME_Y = 6.f;
constexpr float LIVES_X = 525.f;
constexpr float LIVES_Y = 6.f;
constexpr float POWER_X = 16.f;
constexpr float POWER_Y = 22.f;
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
        m_scoreText->setOutlineColor(sf::Color::Black);
        m_scoreText->setOutlineThickness(1.2f);
        m_scoreText->setPosition({SCORE_X, SCORE_Y});

        m_coinText->setCharacterSize(FONT_SIZE);
        m_coinText->setFillColor(sf::Color(255, 215, 0));
        m_coinText->setOutlineColor(sf::Color::Black);
        m_coinText->setOutlineThickness(1.2f);
        m_coinText->setPosition({COINS_X, COINS_Y});

        m_worldText->setCharacterSize(FONT_SIZE);
        m_worldText->setFillColor(sf::Color::White);
        m_worldText->setOutlineColor(sf::Color::Black);
        m_worldText->setOutlineThickness(1.2f);
        m_worldText->setPosition({WORLD_X, WORLD_Y});

        m_timeText->setCharacterSize(FONT_SIZE);
        m_timeText->setFillColor(sf::Color::White);
        m_timeText->setOutlineColor(sf::Color::Black);
        m_timeText->setOutlineThickness(1.2f);
        m_timeText->setPosition({TIME_X, TIME_Y});

        m_livesText->setCharacterSize(FONT_SIZE);
        m_livesText->setFillColor(sf::Color::White);
        m_livesText->setOutlineColor(sf::Color::Black);
        m_livesText->setOutlineThickness(1.2f);
        m_livesText->setPosition({LIVES_X, LIVES_Y});

        m_powerText->setCharacterSize(POWER_FONT_SIZE);
        m_powerText->setFillColor(sf::Color(255, 230, 140));
        m_powerText->setOutlineColor(sf::Color::Black);
        m_powerText->setOutlineThickness(1.f);
        m_powerText->setPosition({POWER_X, POWER_Y});
    }

    // Subscribe to gameplay events that affect the display.
    EventBus& bus = EventBus::getInstance();
    m_eventSubscriptions.reserve(10);
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::COIN_COLLECTED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::PLAYER_DIED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::PLAYER_POWER_UP, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::PLAYER_POWER_DOWN, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::PLAYER_STAR_COLLECTED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::PLAYER_INVINCIBILITY_EXPIRED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::ONE_UP_COLLECTED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::GAME_PAUSED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::LEVEL_COMPLETED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::LEVEL_STARTED, this));

  // Render the initial values.
  refreshText();
}

HUD::~HUD() {
    m_eventSubscriptions.clear();
}

// ── IObserver ────────────────────────────────────────────────

void HUD::onNotify(const GameEvent& eventData) {
    const EventType event = eventData.type;
    switch (event) {
        case EventType::COIN_COLLECTED:
            refreshText();
            break;
        case EventType::PLAYER_DIED:
            m_starPowerActive = false;
            stopTimer();
            refreshText();
            break;
        case EventType::PLAYER_POWER_UP:
            refreshText();
            break;
        case EventType::PLAYER_POWER_DOWN:
            m_starPowerActive = false;
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
            m_starPowerActive = false;
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

void HUD::attachSecondPlayer(const Mario& player) {
    m_mario2 = &player;
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

int HUD::getCoinCount() const {
    int coins = m_mario.getCoinCount();
    if (m_mario2) {
        coins += m_mario2->getCoinCount();
    }
    return coins;
}

std::string HUD::getPowerLabel() const {
    if (m_starPowerActive && m_mario.isStarInvincible()) {
        return "STAR";
    }

    switch (m_mario.getMarioState()) {
        case MarioState::SMALL:
            return "SMALL";
        case MarioState::SUPER:
            return "SUPER";
        case MarioState::FIRE_SMALL:
            return "FIRE SMALL";
        case MarioState::FIRE_SUPER:
            return "FIRE SUPER";
    }

    return "SMALL";
}

std::string HUD::getWorldLabel() const {
    std::ostringstream worldStream;
    worldStream << "WORLD " << m_worldNumber << "-" << m_levelNumber;
    return worldStream.str();
}

std::string HUD::getTimeLabel() const {
    std::ostringstream timeStream;
    timeStream << "TIME " << std::setw(3) << std::setfill('0')
               << m_timeRemaining;
    return timeStream.str();
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

void HUD::stopTimer() {
    m_timerEnabled = false;
    m_timerPausedForEvent = false;
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
    m_font.setSmooth(false);
    return true;
  }

  // Runtime must be portable: never probe absolute system font paths. The
  // packaged font is the only supported font for the release build.
#ifdef DEBUG
  std::cerr << "[DEBUG][HUD] Failed to load packaged font from '" << filepath
            << "'. HUD text is disabled.\n";
#endif
  return false;
}

void HUD::refreshText() {
  if (!m_fontLoaded) {
    return;
  }

    // Format score as a zero-padded 6-digit string (classic Mario style).
    // In co-op the displayed score/coins are the team totals and lives are
    // the shared pool (the minimum across both players).
    int displayScore = m_mario.getScore();
    int displayCoins = m_mario.getCoinCount();
    int displayLives = m_mario.getLives();
    if (m_mario2) {
        displayScore += m_mario2->getScore();
        displayCoins += m_mario2->getCoinCount();
        displayLives = std::min(displayLives, m_mario2->getLives());
    }
    displayScore = std::clamp(displayScore, 0, 999999);

    std::ostringstream scoreStream;
    scoreStream << "SCORE " << std::setw(SCORE_PAD_WIDTH) << std::setfill('0')
                << displayScore;
    m_scoreText->setString(scoreStream.str());

  // Format lives as "LIVES x N".
  std::ostringstream livesStream;
  livesStream << "LIVES x " << displayLives;
  m_livesText->setString(livesStream.str());

  // Format coin count with padding.
  std::ostringstream coinStream;
  coinStream << "COINS x " << std::setw(COIN_PAD_WIDTH) << std::setfill('0')
             << displayCoins;
  m_coinText->setString(coinStream.str());

    // Format world indicator as "WORLD W-L" and timer consistently with the
    // renderer-independent accessors.
    m_worldText->setString(getWorldLabel());
    m_timeText->setString(getTimeLabel());
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
