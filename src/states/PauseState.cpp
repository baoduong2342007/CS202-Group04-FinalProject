/**
 * @file PauseState.cpp
 * @author TV1 (Dương) & TV2 (Nhật)
 * @brief Polished arcade Pause State modal dialog with adaptive highlight box and balanced audio controls.
 */
#include "states/PauseState.h"
#include "core/DisplayConfig.h"
#include "core/GameManager.h"
#include "core/SoundManager.h"
#include "patterns/InputState.h"
#include "states/MenuState.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace {
constexpr unsigned int TITLE_FONT_SIZE = 20;
constexpr unsigned int SUBTITLE_FONT_SIZE = 9;
constexpr unsigned int ITEM_FONT_SIZE = 10;
constexpr unsigned int HINT_FONT_SIZE = 8;

constexpr float PANEL_X = 110.f;
constexpr float PANEL_Y = 48.f;
constexpr float PANEL_WIDTH = 420.f;
constexpr float PANEL_HEIGHT = 264.f;
constexpr std::uint8_t OVERLAY_ALPHA = 180;
constexpr const char* FONT_PATH = "assets/fonts/mario.ttf";

const sf::Color PANEL_COLOR(12, 18, 40, 248);
const sf::Color GOLD_COLOR(255, 215, 0);
const sf::Color CYAN_COLOR(130, 200, 255);
const sf::Color WHITE_COLOR(240, 245, 255);
const sf::Color MUTED_BLUE_COLOR(180, 210, 250);
const sf::Color HIGHLIGHT_BG(24, 40, 88, 220);

std::string makeProgressBar(float percent, int totalBars = 8) {
    int filled = std::clamp(static_cast<int>(std::round(percent / 100.f * totalBars)), 0, totalBars);
    std::string bar = "[";
    for (int i = 0; i < filled; ++i) bar += "=";
    for (int i = filled; i < totalBars; ++i) bar += "-";
    bar += "]";
    return bar;
}
} // namespace

PauseState::PauseState() : m_font(), m_fontLoaded(false) {
    m_fontLoaded = m_font.openFromFile(FONT_PATH);
    if (m_fontLoaded) {
        m_font.setSmooth(false);
    }

    // Modal Box
    m_panel.setSize({PANEL_WIDTH, PANEL_HEIGHT});
    m_panel.setPosition({PANEL_X, PANEL_Y});
    m_panel.setFillColor(PANEL_COLOR);
    m_panel.setOutlineColor(GOLD_COLOR);
    m_panel.setOutlineThickness(-2.5f);

    m_highlightBar.setFillColor(HIGHLIGHT_BG);
    m_highlightBar.setOutlineColor(GOLD_COLOR);
    m_highlightBar.setOutlineThickness(-1.5f);

    initLayout();
}

void PauseState::initLayout() {
    if (!m_fontLoaded) return;

    // Title
    m_titleText.emplace(m_font, "PAUSED", TITLE_FONT_SIZE);
    m_titleText->setFillColor(GOLD_COLOR);
    m_titleText->setOutlineColor(sf::Color::Black);
    m_titleText->setOutlineThickness(1.5f);
    m_titleText->setStyle(sf::Text::Bold);
    sf::FloatRect titleB = m_titleText->getLocalBounds();
    m_titleText->setOrigin({titleB.position.x + titleB.size.x / 2.f, titleB.position.y + titleB.size.y / 2.f});
    m_titleText->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, PANEL_Y + 24.f});

    // Subtitle
    m_subtitleText.emplace(m_font, "AUDIO SETTINGS & OPTIONS", SUBTITLE_FONT_SIZE);
    m_subtitleText->setFillColor(CYAN_COLOR);
    m_subtitleText->setOutlineColor(sf::Color::Black);
    m_subtitleText->setOutlineThickness(1.f);
    sf::FloatRect subB = m_subtitleText->getLocalBounds();
    m_subtitleText->setOrigin({subB.position.x + subB.size.x / 2.f, subB.position.y + subB.size.y / 2.f});
    m_subtitleText->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, PANEL_Y + 44.f});

    // Menu Items (4 items)
    m_menuTexts.clear();
    for (int i = 0; i < 4; ++i) {
        sf::Text item(m_font, "", ITEM_FONT_SIZE);
        item.setOutlineColor(sf::Color::Black);
        item.setOutlineThickness(1.f);
        m_menuTexts.push_back(item);
    }

    // Hint
    m_hintText.emplace(m_font,
        "[UP / DOWN] SELECT    [LEFT / RIGHT] ADJUST VOLUME\n"
        "[ENTER / CLICK] ACTIVATE    [ESC] RESUME",
        HINT_FONT_SIZE);
    m_hintText->setFillColor(MUTED_BLUE_COLOR);
    m_hintText->setOutlineColor(sf::Color::Black);
    m_hintText->setOutlineThickness(1.f);
    m_hintText->setLineSpacing(1.25f);
    sf::FloatRect hintB = m_hintText->getLocalBounds();
    m_hintText->setOrigin({hintB.position.x + hintB.size.x / 2.f, hintB.position.y + hintB.size.y / 2.f});
    m_hintText->setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, PANEL_Y + 234.f});

    refreshVolumeDisplay();
}

void PauseState::refreshVolumeDisplay() {
    if (!m_fontLoaded || m_menuTexts.size() < 4) return;

    const SoundManager& sound = SoundManager::getInstance();
    const float musicVol = sound.getMusicVolume();
    const float sfxVol = sound.getSoundVolume();

    // 0: Music
    std::ostringstream musicStream;
    musicStream << "MUSIC  " << makeProgressBar(musicVol) << "  "
                << std::setw(3) << static_cast<int>(std::round(musicVol)) << "%";

    // 1: SFX (7 chars header matches MUSIC)
    std::ostringstream sfxStream;
    sfxStream << "SFX    " << makeProgressBar(sfxVol) << "  "
              << std::setw(3) << static_cast<int>(std::round(sfxVol)) << "%";

    const std::string labels[4] = {
        musicStream.str(),
        sfxStream.str(),
        "RESUME GAME",
        "QUIT TO MAIN MENU"
    };

    const float startY = PANEL_Y + 76.f;
    const float spacing = 34.f;

    for (int i = 0; i < 4; ++i) {
        const bool selected = (m_selectedIndex == i);
        std::string displayStr;
        if (i < 2) {
            // Equal-width prefix/suffix so slider columns never shift
            displayStr = (selected ? "> " + labels[i] + " <" : "  " + labels[i] + "  ");
        } else {
            displayStr = (selected ? "> " + labels[i] + " <" : labels[i]);
        }
        m_menuTexts[i].setString(displayStr);
        m_menuTexts[i].setCharacterSize(ITEM_FONT_SIZE);

        if (selected) {
            m_menuTexts[i].setFillColor(GOLD_COLOR);
            m_menuTexts[i].setStyle(sf::Text::Bold);
            m_menuTexts[i].setOutlineThickness(1.5f);
        } else {
            m_menuTexts[i].setFillColor(WHITE_COLOR);
            m_menuTexts[i].setStyle(sf::Text::Regular);
            m_menuTexts[i].setOutlineThickness(1.f);
        }

        // Center origin horizontally and vertically
        sf::FloatRect bounds = m_menuTexts[i].getLocalBounds();
        m_menuTexts[i].setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f
        });
        m_menuTexts[i].setPosition({DisplayConfig::LOGICAL_WIDTH / 2.f, startY + i * spacing});
    }

    // Adaptive highlight box that wraps symmetrically around the selected item with balanced margins
    const sf::FloatRect selBounds = m_menuTexts[m_selectedIndex].getLocalBounds();
    const float boxW = selBounds.size.x + 36.f;
    const float boxH = 24.f;

    m_highlightBar.setSize({boxW, boxH});
    m_highlightBar.setOrigin({boxW / 2.f, boxH / 2.f});
    m_highlightBar.setPosition({
        DisplayConfig::LOGICAL_WIDTH / 2.f,
        startY + m_selectedIndex * spacing
    });
}

void PauseState::adjustVolume(int itemIndex, float delta) {
    SoundManager& sound = SoundManager::getInstance();
    if (itemIndex == 0) {
        // Music volume
        const float newVol = std::clamp(sound.getMusicVolume() + delta, 0.f, 100.f);
        sound.setMusicVolume(newVol);
        sound.playSound(SoundId::BUMP);
    } else if (itemIndex == 1) {
        // SFX volume
        const float newVol = std::clamp(sound.getSoundVolume() + delta, 0.f, 100.f);
        sound.setSoundVolume(newVol);
        sound.playSound(SoundId::COIN);
    }

    GameManager::getInstance().getSaveManager().updateAudioSettings(
        sound.getSoundVolume(), sound.getMusicVolume());
    refreshVolumeDisplay();
}

void PauseState::executeSelection(int index) {
    if (index == 0) {
        adjustVolume(0, 10.f);
    } else if (index == 1) {
        adjustVolume(1, 10.f);
    } else if (index == 2) {
        // Resume Game
        SoundManager::getInstance().playSound(SoundId::PAUSE);
        GameManager::getInstance().popState();
    } else if (index == 3) {
        // Quit to Menu
        SoundManager::getInstance().playSound(SoundId::POWER_DOWN);
        GameManager::getInstance().changeState(std::make_unique<MenuState>());
    }
}

void PauseState::onEnter() {
    m_animTimer = 0.f;
    refreshVolumeDisplay();
}

void PauseState::onExit() {}

void PauseState::processEvents(const sf::Event& event) {
    if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
        const sf::Vector2f pos{static_cast<float>(mouseMoved->position.x),
                               static_cast<float>(mouseMoved->position.y)};
        for (int i = 0; i < 4; ++i) {
            sf::FloatRect bounds = m_menuTexts[i].getGlobalBounds();
            sf::FloatRect itemRect(
                {bounds.position.x - 14.f, bounds.position.y - 4.f},
                {bounds.size.x + 28.f, bounds.size.y + 8.f}
            );
            if (itemRect.contains(pos)) {
                if (m_selectedIndex != i) {
                    m_selectedIndex = i;
                    SoundManager::getInstance().playSound(SoundId::BUMP);
                    refreshVolumeDisplay();
                }
                break;
            }
        }
    }

    if (const auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseButton->button == sf::Mouse::Button::Left) {
            const sf::Vector2f pos{static_cast<float>(mouseButton->position.x),
                                   static_cast<float>(mouseButton->position.y)};
            for (int i = 0; i < 4; ++i) {
                sf::FloatRect bounds = m_menuTexts[i].getGlobalBounds();
                sf::FloatRect itemRect(
                    {bounds.position.x - 14.f, bounds.position.y - 4.f},
                    {bounds.size.x + 28.f, bounds.size.y + 8.f}
                );
                if (itemRect.contains(pos)) {
                    if (i == 0 || i == 1) {
                        // Check if clicked on left half (decrease) or right half (increase)
                        if (pos.x < DisplayConfig::LOGICAL_WIDTH / 2.f) {
                            adjustVolume(i, -10.f);
                        } else {
                            adjustVolume(i, +10.f);
                        }
                    } else {
                        executeSelection(i);
                    }
                    break;
                }
            }
        }
    }
}

void PauseState::processInput(const InputState& inputState) {
    if (inputState.wasPressed(sf::Keyboard::Key::Escape)) {
        SoundManager::getInstance().playSound(SoundId::PAUSE);
        GameManager::getInstance().popState();
        return;
    }

    // Vertical navigation between ALL 4 items
    if (inputState.wasPressed(sf::Keyboard::Key::Up) ||
        inputState.wasPressed(sf::Keyboard::Key::W)) {
        m_selectedIndex = (m_selectedIndex + 3) % 4;
        SoundManager::getInstance().playSound(SoundId::BUMP);
        refreshVolumeDisplay();
    } else if (inputState.wasPressed(sf::Keyboard::Key::Down) ||
               inputState.wasPressed(sf::Keyboard::Key::S)) {
        m_selectedIndex = (m_selectedIndex + 1) % 4;
        SoundManager::getInstance().playSound(SoundId::BUMP);
        refreshVolumeDisplay();
    }

    // Horizontal adjustment for volume sliders
    if (inputState.wasPressed(sf::Keyboard::Key::Left) ||
        inputState.wasPressed(sf::Keyboard::Key::A)) {
        if (m_selectedIndex == 0) {
            adjustVolume(0, -5.f);
        } else if (m_selectedIndex == 1) {
            adjustVolume(1, -5.f);
        }
    } else if (inputState.wasPressed(sf::Keyboard::Key::Right) ||
               inputState.wasPressed(sf::Keyboard::Key::D)) {
        if (m_selectedIndex == 0) {
            adjustVolume(0, +5.f);
        } else if (m_selectedIndex == 1) {
            adjustVolume(1, +5.f);
        }
    }

    // Activation
    if (inputState.wasPressed(sf::Keyboard::Key::Enter) ||
        inputState.wasPressed(sf::Keyboard::Key::Space)) {
        executeSelection(m_selectedIndex);
    }
}

void PauseState::update(float dt) {
    m_animTimer += dt;

    const float pulse = (std::sin(m_animTimer * 6.f) + 1.f) * 0.5f;
    const sf::Color activeGold(
        static_cast<std::uint8_t>(220 + 35 * pulse),
        static_cast<std::uint8_t>(180 + 35 * pulse),
        0
    );

    m_highlightBar.setOutlineColor(activeGold);
}

void PauseState::render(sf::RenderTarget& target) {
    target.setView(target.getDefaultView());

    // Darkened background overlay
    sf::RectangleShape overlay(sf::Vector2f(target.getSize().x, target.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, OVERLAY_ALPHA));
    target.draw(overlay);

    // Modal Box
    target.draw(m_panel);
    target.draw(m_highlightBar);

    if (!m_fontLoaded) return;

    if (m_titleText) target.draw(*m_titleText);
    if (m_subtitleText) target.draw(*m_subtitleText);

    for (const auto& itemText : m_menuTexts) {
        target.draw(itemText);
    }

    if (m_hintText) target.draw(*m_hintText);
}
