/**
 * @file HUD.cpp
 * @author TV5 (Truyền)
 * @brief Implementation of HUD — score/lives display with pixel font.
 * @note sf::Font::loadFromFile returns bool (unlike sf::Texture/sf::SoundBuffer
 *       constructors in SFML 3 which throw). The return value is checked and
 *       the HUD degrades gracefully if the font file is missing.
 */

#include "ui/HUD.h"

#include <iostream>
#include <sstream>
#include <iomanip>

// ============================================================
// PATTERN: Observer (Subscriber)
// Reason: HUD subscribes to gameplay events so it can refresh its
//         display without being coupled to the event publishers.
// ============================================================

namespace {
// ── Constants ────────────────────────────────────────────────
constexpr const char* FONT_PATH = "assets/fonts/mario.ttf";
constexpr unsigned int FONT_SIZE = 24;
constexpr int STARTING_LIVES = 3;
constexpr float SCORE_X = 20.f;
constexpr float SCORE_Y = 10.f;
constexpr float LIVES_X = 600.f;
constexpr float LIVES_Y = 10.f;
} // namespace

// ── Constructor / Destructor ─────────────────────────────────

HUD::HUD(const Mario& mario)
    : m_mario(mario),
      m_lives(STARTING_LIVES),
      m_fontLoaded(false) {
    // Load the pixel font; check the bool return value.
    m_fontLoaded = loadFont(FONT_PATH);

    // SFML 3: sf::Text requires a font at construction time, so we
    // only construct the text objects when the font is available.
    if (m_fontLoaded) {
        m_scoreText.emplace(m_font);
        m_livesText.emplace(m_font);

        m_scoreText->setCharacterSize(FONT_SIZE);
        m_scoreText->setFillColor(sf::Color::White);
        m_scoreText->setPosition({SCORE_X, SCORE_Y});

        m_livesText->setCharacterSize(FONT_SIZE);
        m_livesText->setFillColor(sf::Color::White);
        m_livesText->setPosition({LIVES_X, LIVES_Y});
    }

    // Subscribe to gameplay events that affect the display.
    EventBus& bus = EventBus::getInstance();
    bus.subscribe(EventType::COIN_COLLECTED, this);
    bus.subscribe(EventType::PLAYER_DIED, this);
    bus.subscribe(EventType::PLAYER_POWER_UP, this);

    // Render the initial values.
    refreshText();
}

HUD::~HUD() {
    // Observer contract: unsubscribe to avoid dangling pointers.
    EventBus& bus = EventBus::getInstance();
    bus.unsubscribe(EventType::COIN_COLLECTED, this);
    bus.unsubscribe(EventType::PLAYER_DIED, this);
    bus.unsubscribe(EventType::PLAYER_POWER_UP, this);
}

// ── IObserver ────────────────────────────────────────────────

void HUD::onNotify(EventType event) {
    switch (event) {
        case EventType::COIN_COLLECTED:
            // Score changed — re-read from Mario.
            refreshText();
            break;
        case EventType::PLAYER_DIED:
            // Decrement lives and refresh the display.
            if (m_lives > 0) {
                --m_lives;
            }
            refreshText();
            break;
        case EventType::PLAYER_POWER_UP:
            // Power-up may change score; refresh to be safe.
            refreshText();
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

void HUD::draw(sf::RenderTarget& target) const {
    // Only draw if the font loaded; otherwise the text is invisible.
    if (m_fontLoaded) {
        target.draw(*m_scoreText);
        target.draw(*m_livesText);
    }
}

// ── Getters / Setters ────────────────────────────────────────

int HUD::getLives() const {
    return m_lives;
}

void HUD::setLives(int lives) {
    m_lives = lives;
    refreshText();
}

// ── Private methods ──────────────────────────────────────────

bool HUD::loadFont(const std::string& filepath) {
    // SFML 3: sf::Font::openFromFile returns bool — check it explicitly.
    if (!m_font.openFromFile(filepath)) {
        std::cerr << "[HUD] WARNING: Failed to load font from '"
                  << filepath << "'. HUD text will not be drawn.\n";
        return false;
    }
    return true;
}

void HUD::refreshText() {
    if (!m_fontLoaded) {
        return;
    }

    // Format score as a zero-padded 6-digit string (classic Mario style).
    std::ostringstream scoreStream;
    scoreStream << "SCORE " << std::setw(6) << std::setfill('0')
                << m_mario.getScore();
    m_scoreText->setString(scoreStream.str());

    // Format lives as "LIVES x N".
    std::ostringstream livesStream;
    livesStream << "LIVES x " << m_lives;
    m_livesText->setString(livesStream.str());
}
