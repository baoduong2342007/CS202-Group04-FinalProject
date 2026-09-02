/**
 * @file RecordsState.cpp
 * @author Group 04
 * @brief Polished Hall of Fame / Game Records & Match History State with mode indicator and screenshot capabilities.
 */

#include "states/RecordsState.h"
#include "states/MenuState.h"
#include "core/DisplayConfig.h"
#include "core/GameManager.h"
#include "core/LevelCatalog.h"
#include "core/SoundManager.h"
#include "patterns/InputState.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace {
constexpr const char* FONT_PATH = "assets/fonts/mario.ttf";

const sf::Color BG_COLOR(12, 16, 38);
const sf::Color PANEL_COLOR(8, 12, 28);
const sf::Color TABLE_BG_COLOR(5, 7, 18);
const sf::Color TABLE_HEADER_BG(14, 20, 46);
const sf::Color BORDER_COLOR(45, 65, 120);
const sf::Color GOLD_COLOR(255, 215, 0);
const sf::Color MUTED_GOLD_COLOR(255, 230, 130);
const sf::Color CYAN_COLOR(130, 200, 255);
const sf::Color GREEN_COLOR(100, 240, 120);
const sf::Color RED_COLOR(240, 90, 90);
const sf::Color BODY_COLOR(235, 240, 255);
const sf::Color ROW_BG_1(14, 22, 48);
const sf::Color ROW_BG_2(9, 14, 32);

const sf::Color THEME_COLORS[4] = {
    sf::Color(46, 125, 50),   // World 1-1 Overworld
    sf::Color(21, 101, 192),  // World 1-2 Underground
    sf::Color(0, 131, 143),   // World 1-3 Underwater
    sf::Color(183, 28, 28)    // World 1-4 Castle
};

// Column X Positions for the 7 columns in the table (Total W=574, Start X=33)
constexpr float COL_STAGE = 44.f;  // WORLD 1-1 / ARENA
constexpr float COL_MODE  = 124.f; // SOLO / CO-OP / PVP
constexpr float COL_HERO  = 180.f; // MARIO / LUIGI / P1 MARIO
constexpr float COL_RES   = 264.f; // CLEARED / GAME OVER / P1 WIN
constexpr float COL_SCORE = 348.f; // 014500
constexpr float COL_COINS = 414.f; // X08
constexpr float COL_DATE  = 464.f; // 2026-09-02 17:05

void centerText(sf::Text& text, float centerX, float centerY) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin({
        std::floor(bounds.position.x + bounds.size.x / 2.f),
        std::floor(bounds.position.y + bounds.size.y / 2.f)
    });
    text.setPosition({std::floor(centerX), std::floor(centerY)});
}
} // namespace

RecordsState::RecordsState() = default;

void RecordsState::onEnter() {
    m_transitioning = false;
    m_animTimer = 0.f;
    m_notificationTimer = 0.f;
    m_scrollOffset = 0;
    m_selectedIndex = 0;
    m_confirmingReset = false;
    m_confirmingDeleteSingle = false;

    m_fontLoaded = m_font.openFromFile(FONT_PATH);
    if (m_fontLoaded) {
        m_font.setSmooth(false);
    }

    initBackdropPanel();
    initTextLabels();
    initStageCards();
    updateMatchRows();
}

void RecordsState::onExit() {}

void RecordsState::initBackdropPanel() {
    m_panel.setSize({604.f, 340.f});
    m_panel.setPosition({18.f, 10.f});
    m_panel.setFillColor(PANEL_COLOR);
    m_panel.setOutlineColor(GOLD_COLOR);
    m_panel.setOutlineThickness(2.f);
}

void RecordsState::initTextLabels() {
    if (!m_fontLoaded) return;

    // Main Title (Pure ASCII - centered at X=320, Y=24)
    m_titleText.emplace(m_font, "- HALL OF FAME & RECORDS -", 15);
    m_titleText->setFillColor(GOLD_COLOR);
    m_titleText->setOutlineColor(sf::Color::Black);
    m_titleText->setOutlineThickness(1.5f);
    m_titleText->setStyle(sf::Text::Bold);
    centerText(*m_titleText, DisplayConfig::LOGICAL_WIDTH / 2.f, 24.f);

    // Subtitle (centered at X=320, Y=40)
    m_subtitleText.emplace(m_font, "[ STAGE HIGH SCORES & RECENT MATCH LOG ]", 8);
    m_subtitleText->setFillColor(CYAN_COLOR);
    m_subtitleText->setOutlineColor(sf::Color::Black);
    m_subtitleText->setOutlineThickness(1.f);
    centerText(*m_subtitleText, DisplayConfig::LOGICAL_WIDTH / 2.f, 40.f);

    // Section 1 Header
    m_section1Title.emplace(m_font, "STAGE BEST SCORES:", 8);
    m_section1Title->setFillColor(MUTED_GOLD_COLOR);
    m_section1Title->setPosition({32.f, 52.f});

    // Section 2 Header
    m_section2Title.emplace(m_font, "RECENT ADVENTURES LOG:", 8);
    m_section2Title->setFillColor(MUTED_GOLD_COLOR);
    m_section2Title->setPosition({32.f, 130.f});

    // Empty History Message
    m_emptyText.emplace(m_font, "[ NO MATCH RECORDS YET. PLAY A GAME TO RECORD YOUR RUN! ]", 8);
    m_emptyText->setFillColor(sf::Color(140, 175, 220));
    centerText(*m_emptyText, DisplayConfig::LOGICAL_WIDTH / 2.f, 228.f);

    // Footer Hint
    m_hintText.emplace(m_font, "[UP / DN] SELECT   [X / DEL] DELETE ENTRY   [C] RESET ALL   [P] SCREENSHOT   [ESC] BACK", 8);
    m_hintText->setFillColor(sf::Color(180, 210, 250));
    centerText(*m_hintText, DisplayConfig::LOGICAL_WIDTH / 2.f, 327.f);

    // Notification Text (e.g. Screenshot saved)
    m_notificationText.emplace(m_font, "", 8);
    m_notificationText->setFillColor(GREEN_COLOR);
    m_notificationText->setOutlineColor(sf::Color::Black);
    m_notificationText->setOutlineThickness(1.2f);
}

void RecordsState::initStageCards() {
    m_stageCards.clear();
    const auto& saveManager = GameManager::getInstance().getSaveManager();
    const int highestUnlocked = saveManager.getData().highestUnlockedLevel;

    constexpr float CARD_W = 136.f;
    constexpr float CARD_H = 58.f;
    constexpr float START_X = 32.f;
    constexpr float SPACING = 10.f;
    constexpr float CARD_Y = 64.f;

    for (int i = 0; i < 4; ++i) {
        const int levelNum = i + 1;
        StageRecordCard card;
        card.levelNumber = levelNum;

        const float cardX = START_X + i * (CARD_W + SPACING);
        const float centerX = cardX + CARD_W / 2.f;

        // Outer Card Box
        card.outerCard.setSize({CARD_W, CARD_H});
        card.outerCard.setPosition({cardX, CARD_Y});
        card.outerCard.setFillColor(sf::Color(16, 22, 48));
        card.outerCard.setOutlineColor(BORDER_COLOR);
        card.outerCard.setOutlineThickness(1.f);

        // Header Banner
        card.headerBanner.setSize({CARD_W, 18.f});
        card.headerBanner.setPosition({cardX, CARD_Y});
        card.headerBanner.setFillColor(THEME_COLORS[i % 4]);

        if (m_fontLoaded) {
            // Title (e.g. "WORLD 1-1") - centered inside banner
            card.titleText.emplace(m_font, "WORLD 1-" + std::to_string(levelNum), 9);
            card.titleText->setFillColor(sf::Color::White);
            card.titleText->setOutlineColor(sf::Color::Black);
            card.titleText->setOutlineThickness(1.f);
            card.titleText->setStyle(sf::Text::Bold);
            centerText(*card.titleText, centerX, CARD_Y + 9.f);

            // Status (CLEARED, UNLOCKED, LOCKED)
            std::string statusStr = "LOCKED";
            sf::Color statusCol = RED_COLOR;
            if (levelNum < highestUnlocked) {
                statusStr = "CLEARED";
                statusCol = GREEN_COLOR;
            } else if (levelNum == highestUnlocked) {
                statusStr = "UNLOCKED";
                statusCol = CYAN_COLOR;
            }
            card.statusText.emplace(m_font, statusStr, 7);
            card.statusText->setFillColor(statusCol);
            card.statusText->setOutlineColor(sf::Color::Black);
            card.statusText->setOutlineThickness(0.8f);
            centerText(*card.statusText, centerX, CARD_Y + 27.f);

            // High Score (e.g. "BEST: 014500")
            const int stageBest = saveManager.getLevelHighScore(levelNum);
            std::string scoreStr = std::to_string(stageBest);
            while (scoreStr.length() < 6) scoreStr = "0" + scoreStr;
            card.bestScoreText.emplace(m_font, "BEST: " + scoreStr, 8);
            card.bestScoreText->setFillColor(GOLD_COLOR);
            card.bestScoreText->setOutlineColor(sf::Color::Black);
            card.bestScoreText->setOutlineThickness(1.f);
            centerText(*card.bestScoreText, centerX, CARD_Y + 43.f);
        }

        m_stageCards.push_back(std::move(card));
    }
}

void RecordsState::updateMatchRows() {
    m_matchRows.clear();
    if (!m_fontLoaded) return;

    const auto& history = GameManager::getInstance().getSaveManager().getMatchHistory();
    if (history.empty()) return;

    constexpr int VISIBLE_ROWS = 5;
    constexpr float ROW_START_Y = 168.f;
    constexpr float ROW_H = 23.f;
    constexpr float TABLE_X = 33.f;
    constexpr float TABLE_W = 574.f;

    const int total = static_cast<int>(history.size());
    m_selectedIndex = std::clamp(m_selectedIndex, 0, std::max(0, total - 1));

    // Keep selected row visible within the 5 visible rows
    if (m_selectedIndex < m_scrollOffset) {
        m_scrollOffset = m_selectedIndex;
    } else if (m_selectedIndex >= m_scrollOffset + VISIBLE_ROWS) {
        m_scrollOffset = m_selectedIndex - VISIBLE_ROWS + 1;
    }
    m_scrollOffset = std::clamp(m_scrollOffset, 0, std::max(0, total - VISIBLE_ROWS));

    for (int i = 0; i < VISIBLE_ROWS && (m_scrollOffset + i) < total; ++i) {
        const int recordIdx = m_scrollOffset + i;
        const auto& rec = history[recordIdx];
        const float rowY = ROW_START_Y + i * (ROW_H + 2.f);
        const bool isSelected = (recordIdx == m_selectedIndex);

        MatchRow row;
        row.rowBg.setSize({TABLE_W - 2.f, ROW_H});
        row.rowBg.setPosition({TABLE_X + 1.f, rowY});
        if (isSelected) {
            row.rowBg.setFillColor(sf::Color(25, 42, 85));
            row.rowBg.setOutlineColor(GOLD_COLOR);
            row.rowBg.setOutlineThickness(1.5f);
        } else {
            row.rowBg.setFillColor((i % 2 == 0) ? ROW_BG_1 : ROW_BG_2);
            row.rowBg.setOutlineColor(sf::Color(30, 45, 85));
            row.rowBg.setOutlineThickness(0.5f);
        }

        const float textY = rowY + 5.f;

        // Stage
        std::string stageLabel = (rec.mode == "PVP") ? "ARENA" : ("WORLD 1-" + std::to_string(rec.level));
        if (isSelected) {
            row.stageText.emplace(m_font, "> " + stageLabel, 8);
            row.stageText->setFillColor(GOLD_COLOR);
            row.stageText->setPosition({COL_STAGE - 8.f, textY});
        } else {
            row.stageText.emplace(m_font, stageLabel, 8);
            row.stageText->setFillColor(CYAN_COLOR);
            row.stageText->setPosition({COL_STAGE, textY});
        }

        // Mode (SOLO, CO-OP, PVP)
        std::string modeUpper = rec.mode.empty() ? "SOLO" : rec.mode;
        std::transform(modeUpper.begin(), modeUpper.end(), modeUpper.begin(), ::toupper);
        row.modeText.emplace(m_font, modeUpper, 8);
        if (modeUpper == "CO-OP") {
            row.modeText->setFillColor(sf::Color(255, 175, 235));
        } else if (modeUpper == "PVP") {
            row.modeText->setFillColor(sf::Color(255, 170, 70));
        } else {
            row.modeText->setFillColor(sf::Color(150, 215, 255));
        }
        row.modeText->setPosition({COL_MODE, textY});

        // Hero
        std::string heroUpper = rec.character;
        std::transform(heroUpper.begin(), heroUpper.end(), heroUpper.begin(), ::toupper);
        row.heroText.emplace(m_font, heroUpper, 8);
        row.heroText->setFillColor(BODY_COLOR);
        row.heroText->setPosition({COL_HERO, textY});

        // Result
        const bool win = (rec.result == "CLEARED" || rec.result == "P1 WIN" || rec.result == "P2 WIN");
        row.resultText.emplace(m_font, rec.result, 8);
        row.resultText->setFillColor(win ? GREEN_COLOR : RED_COLOR);
        row.resultText->setStyle(sf::Text::Bold);
        row.resultText->setPosition({COL_RES, textY});

        // Score
        std::string scoreStr = std::to_string(rec.score);
        while (scoreStr.length() < 6) scoreStr = "0" + scoreStr;
        row.scoreText.emplace(m_font, scoreStr, 8);
        row.scoreText->setFillColor(GOLD_COLOR);
        row.scoreText->setPosition({COL_SCORE, textY});

        // Coins
        std::string coinStr = std::to_string(rec.coins);
        while (coinStr.length() < 2) coinStr = "0" + coinStr;
        row.coinsText.emplace(m_font, "X" + coinStr, 8);
        row.coinsText->setFillColor(MUTED_GOLD_COLOR);
        row.coinsText->setPosition({COL_COINS, textY});

        // Date
        row.dateText.emplace(m_font, rec.date, 8);
        row.dateText->setFillColor(sf::Color(160, 180, 215));
        row.dateText->setPosition({COL_DATE, textY});

        m_matchRows.push_back(std::move(row));
    }
}

void RecordsState::processEvents(const sf::Event& event) {
    if (m_transitioning) return;

    if (const auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>()) {
        const auto& history = GameManager::getInstance().getSaveManager().getMatchHistory();
        const int total = static_cast<int>(history.size());
        if (total > 0) {
            if (wheel->delta > 0) {
                if (m_selectedIndex > 0) {
                    m_selectedIndex--;
                    m_confirmingDeleteSingle = false;
                    m_confirmingReset = false;
                    updateMatchRows();
                    SoundManager::getInstance().playSound(SoundId::BUMP);
                }
            } else if (wheel->delta < 0) {
                if (m_selectedIndex + 1 < total) {
                    m_selectedIndex++;
                    m_confirmingDeleteSingle = false;
                    m_confirmingReset = false;
                    updateMatchRows();
                    SoundManager::getInstance().playSound(SoundId::BUMP);
                }
            }
        }
    }

    if (const auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseButton->button == sf::Mouse::Button::Left) {
            const sf::Vector2f logicalPos{
                static_cast<float>(mouseButton->position.x),
                static_cast<float>(mouseButton->position.y)};
            for (std::size_t i = 0; i < m_matchRows.size(); ++i) {
                if (m_matchRows[i].rowBg.getGlobalBounds().contains(logicalPos)) {
                    m_selectedIndex = m_scrollOffset + static_cast<int>(i);
                    m_confirmingDeleteSingle = false;
                    m_confirmingReset = false;
                    updateMatchRows();
                    SoundManager::getInstance().playSound(SoundId::BUMP);
                    break;
                }
            }
        }
    }
}

void RecordsState::processInput(const InputState& inputState) {
    if (m_transitioning) return;

    const auto& history = GameManager::getInstance().getSaveManager().getMatchHistory();
    const int total = static_cast<int>(history.size());

    if (inputState.wasPressed(sf::Keyboard::Key::Up) || inputState.wasPressed(sf::Keyboard::Key::W)) {
        if (total > 0 && m_selectedIndex > 0) {
            m_selectedIndex--;
            m_confirmingDeleteSingle = false;
            m_confirmingReset = false;
            updateMatchRows();
            SoundManager::getInstance().playSound(SoundId::BUMP);
        }
    } else if (inputState.wasPressed(sf::Keyboard::Key::Down) || inputState.wasPressed(sf::Keyboard::Key::S)) {
        if (total > 0 && m_selectedIndex + 1 < total) {
            m_selectedIndex++;
            m_confirmingDeleteSingle = false;
            m_confirmingReset = false;
            updateMatchRows();
            SoundManager::getInstance().playSound(SoundId::BUMP);
        }
    } else if (inputState.wasPressed(sf::Keyboard::Key::X) || inputState.wasPressed(sf::Keyboard::Key::Delete)) {
        if (history.empty()) {
            SoundManager::getInstance().playSound(SoundId::BUMP);
            return;
        }
        m_confirmingReset = false;
        if (m_confirmingDeleteSingle) {
            // Second press — actually delete selected entry
            m_confirmingDeleteSingle = false;
            if (m_selectedIndex >= 0 && m_selectedIndex < total) {
                if (GameManager::getInstance().getSaveManager().deleteGameRecord(static_cast<std::size_t>(m_selectedIndex))) {
                    const auto& newHistory = GameManager::getInstance().getSaveManager().getMatchHistory();
                    if (m_selectedIndex >= static_cast<int>(newHistory.size())) {
                        m_selectedIndex = std::max(0, static_cast<int>(newHistory.size()) - 1);
                    }
                    updateMatchRows();
                    SoundManager::getInstance().playSound(SoundId::BRICK);
                    if (m_notificationText) {
                        m_notificationText->setString("SELECTED RECORD DELETED!");
                        m_notificationText->setFillColor(GREEN_COLOR);
                        centerText(*m_notificationText, DisplayConfig::LOGICAL_WIDTH / 2.f, 327.f);
                    }
                    m_notificationTimer = 3.0f;
                }
            }
        } else {
            // First press — show confirmation warning
            m_confirmingDeleteSingle = true;
            SoundManager::getInstance().playSound(SoundId::BUMP);
            if (m_notificationText && m_selectedIndex >= 0 && m_selectedIndex < total) {
                std::string scoreStr = std::to_string(history[m_selectedIndex].score);
                while (scoreStr.length() < 6) scoreStr = "0" + scoreStr;
                m_notificationText->setString("DELETE ENTRY #" + std::to_string(m_selectedIndex + 1) + " (SCORE: " + scoreStr + ")? PRESS [X/DEL] AGAIN  /  [ESC] CANCEL");
                m_notificationText->setFillColor(RED_COLOR);
                centerText(*m_notificationText, DisplayConfig::LOGICAL_WIDTH / 2.f, 327.f);
            }
            m_notificationTimer = 15.0f;
        }
    } else if (inputState.wasPressed(sf::Keyboard::Key::C)) {
        m_confirmingDeleteSingle = false;
        if (m_confirmingReset) {
            // Second press — actually reset all data and level progress
            m_confirmingReset = false;
            if (GameManager::getInstance().getSaveManager().resetAllData()) {
                m_selectedIndex = 0;
                m_scrollOffset = 0;
                initStageCards();
                updateMatchRows();
                SoundManager::getInstance().playSound(SoundId::BRICK);
                if (m_notificationText) {
                    m_notificationText->setString("ALL RECORDS & PROGRESS RESET TO DEFAULT!");
                    m_notificationText->setFillColor(GREEN_COLOR);
                    centerText(*m_notificationText, DisplayConfig::LOGICAL_WIDTH / 2.f, 327.f);
                }
                m_notificationTimer = 3.0f;
            }
        } else {
            // First press — show confirmation warning
            m_confirmingReset = true;
            SoundManager::getInstance().playSound(SoundId::BUMP);
            if (m_notificationText) {
                m_notificationText->setString("RESET ALL DATA & PROGRESS? PRESS [C] AGAIN  /  [ESC] CANCEL");
                m_notificationText->setFillColor(RED_COLOR);
                centerText(*m_notificationText, DisplayConfig::LOGICAL_WIDTH / 2.f, 327.f);
            }
            m_notificationTimer = 15.0f;
        }
    } else if (inputState.wasPressed(sf::Keyboard::Key::F12) || inputState.wasPressed(sf::Keyboard::Key::P) || inputState.wasPressed(sf::Keyboard::Key::F10)) {
        m_confirmingReset = false;
        m_confirmingDeleteSingle = false;
        if (m_notificationText) {
            m_notificationText->setString("SCREENSHOT CAPTURED (SAVED TO SCREENSHOTS/)");
            m_notificationText->setFillColor(GREEN_COLOR);
            centerText(*m_notificationText, DisplayConfig::LOGICAL_WIDTH / 2.f, 327.f);
        }
        m_notificationTimer = 3.0f;
    } else if (inputState.wasPressed(sf::Keyboard::Key::Escape) || inputState.wasPressed(sf::Keyboard::Key::Enter) || inputState.wasPressed(sf::Keyboard::Key::Space)) {
        if (m_confirmingReset || m_confirmingDeleteSingle) {
            // Cancel confirmation instead of leaving
            m_confirmingReset = false;
            m_confirmingDeleteSingle = false;
            m_notificationTimer = 0.f;
            SoundManager::getInstance().playSound(SoundId::BUMP);
            return;
        }
        m_transitioning = true;
        SoundManager::getInstance().playSound(SoundId::POWER_DOWN);
        GameManager::getInstance().changeState(std::make_unique<MenuState>());
    }
}

void RecordsState::update(float dt) {
    m_animTimer += dt;
    if (m_notificationTimer > 0.f) {
        m_notificationTimer -= dt;
        if (m_notificationTimer <= 0.f) {
            m_confirmingReset = false;
            m_confirmingDeleteSingle = false;
        }
    }
}

void RecordsState::render(sf::RenderTarget& target) {
    target.clear(BG_COLOR);
    target.setView(target.getDefaultView());

    // Main Background Panel
    target.draw(m_panel);

    // Section 1: Stage Cards
    for (const auto& card : m_stageCards) {
        target.draw(card.outerCard);
        target.draw(card.headerBanner);
        if (m_fontLoaded) {
            if (card.titleText) target.draw(*card.titleText);
            if (card.statusText) target.draw(*card.statusText);
            if (card.bestScoreText) target.draw(*card.bestScoreText);
        }
    }

    // Section 2: Match History Table Frame
    constexpr float TABLE_X = 33.f;
    constexpr float TABLE_Y = 144.f;
    constexpr float TABLE_W = 574.f;
    constexpr float TABLE_H = 154.f;

    sf::RectangleShape tableBox({TABLE_W, TABLE_H});
    tableBox.setPosition({TABLE_X, TABLE_Y});
    tableBox.setFillColor(TABLE_BG_COLOR);
    tableBox.setOutlineColor(BORDER_COLOR);
    tableBox.setOutlineThickness(1.f);
    target.draw(tableBox);

    // Table Header Bar
    sf::RectangleShape tableHeaderBar({TABLE_W, 20.f});
    tableHeaderBar.setPosition({TABLE_X, TABLE_Y});
    tableHeaderBar.setFillColor(TABLE_HEADER_BG);
    target.draw(tableHeaderBar);

    // Table Header Columns (7 columns exactly aligned with data rows)
    if (m_fontLoaded) {
        const float headerTextY = TABLE_Y + 4.f;

        sf::Text col1(m_font, "STAGE", 8);
        col1.setFillColor(MUTED_GOLD_COLOR);
        col1.setPosition({COL_STAGE, headerTextY});
        target.draw(col1);

        sf::Text col2(m_font, "MODE", 8);
        col2.setFillColor(MUTED_GOLD_COLOR);
        col2.setPosition({COL_MODE, headerTextY});
        target.draw(col2);

        sf::Text col3(m_font, "HERO", 8);
        col3.setFillColor(MUTED_GOLD_COLOR);
        col3.setPosition({COL_HERO, headerTextY});
        target.draw(col3);

        sf::Text col4(m_font, "RESULT", 8);
        col4.setFillColor(MUTED_GOLD_COLOR);
        col4.setPosition({COL_RES, headerTextY});
        target.draw(col4);

        sf::Text col5(m_font, "SCORE", 8);
        col5.setFillColor(MUTED_GOLD_COLOR);
        col5.setPosition({COL_SCORE, headerTextY});
        target.draw(col5);

        sf::Text col6(m_font, "COINS", 8);
        col6.setFillColor(MUTED_GOLD_COLOR);
        col6.setPosition({COL_COINS, headerTextY});
        target.draw(col6);

        sf::Text col7(m_font, "DATE & TIME", 8);
        col7.setFillColor(MUTED_GOLD_COLOR);
        col7.setPosition({COL_DATE, headerTextY});
        target.draw(col7);
    }

    // Match Rows
    const auto& history = GameManager::getInstance().getSaveManager().getMatchHistory();
    if (history.empty()) {
        if (m_fontLoaded && m_emptyText) {
            target.draw(*m_emptyText);
        }
    } else {
        for (const auto& row : m_matchRows) {
            target.draw(row.rowBg);
            if (m_fontLoaded) {
                if (row.stageText) target.draw(*row.stageText);
                if (row.modeText) target.draw(*row.modeText);
                if (row.heroText) target.draw(*row.heroText);
                if (row.resultText) target.draw(*row.resultText);
                if (row.scoreText) target.draw(*row.scoreText);
                if (row.coinsText) target.draw(*row.coinsText);
                if (row.dateText) target.draw(*row.dateText);
            }
        }
    }

    // Header Labels & Hints
    if (m_fontLoaded) {
        if (m_titleText) target.draw(*m_titleText);
        if (m_subtitleText) target.draw(*m_subtitleText);
        if (m_section1Title) target.draw(*m_section1Title);
        if (m_section2Title) target.draw(*m_section2Title);

        // Flash Notification or Regular Hint
        if (m_notificationTimer > 0.f && m_notificationText) {
            target.draw(*m_notificationText);
        } else if (m_hintText) {
            target.draw(*m_hintText);
        }
    }
}
