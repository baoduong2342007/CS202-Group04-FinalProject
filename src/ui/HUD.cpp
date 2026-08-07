/**
 * @file HUD.cpp
 * @author TV5 (Truyền)
 * @brief Implementation of HUD — score, lives, coin count, and world indicator.
 * @note sf::Font::loadFromFile returns bool (unlike sf::Texture/sf::SoundBuffer
 *       constructors in SFML 3 which throw). The return value is checked and
 *       the HUD degrades gracefully if the font file is missing.
 */

#include "ui/HUD.h"

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
  }

  // Subscribe to gameplay events that affect the display.
  EventBus &bus = EventBus::getInstance();
  bus.subscribe(EventType::COIN_COLLECTED, this);
  bus.subscribe(EventType::PLAYER_DIED, this);
  bus.subscribe(EventType::PLAYER_LOST_LIFE, this);
  bus.subscribe(EventType::PLAYER_POWER_UP, this);

  // Render the initial values.
  refreshText();
}

HUD::~HUD() {
  // Observer contract: unsubscribe to avoid dangling pointers.
  EventBus &bus = EventBus::getInstance();
  bus.unsubscribe(EventType::COIN_COLLECTED, this);
  bus.unsubscribe(EventType::PLAYER_DIED, this);
  bus.unsubscribe(EventType::PLAYER_LOST_LIFE, this);
  bus.unsubscribe(EventType::PLAYER_POWER_UP, this);
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
  default:
    break;
  }
}

// ── Public methods ───────────────────────────────────────────

void HUD::update() {
  // Re-read authoritative score from Mario each frame.
  refreshText();
}

void HUD::draw(sf::RenderTarget &target) const {
  // Only draw if the font loaded; otherwise the text is invisible.
  if (m_fontLoaded) {
    target.draw(*m_scoreText);
    target.draw(*m_livesText);
    target.draw(*m_coinText);
    target.draw(*m_worldText);
  }
}

// ── Getters / Setters ────────────────────────────────────────

int HUD::getCoinCount() const { return m_mario.getCoinCount(); }

void HUD::setWorldLevel(int world, int level) {
  m_worldNumber = world;
  m_levelNumber = level;
  refreshText();
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

  // Format score as a zero-padded string (classic Mario style).
  std::ostringstream scoreStream;
  scoreStream << "SCORE " << std::setw(SCORE_PAD_WIDTH) << std::setfill('0')
              << m_mario.getScore();
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
}
