/**
 * @file PauseState.h
 * @author TV1 (Duong) & TV2 (Nhat)
 * @brief Polished arcade Pause State modal dialog with unified audio controls.
 */
#pragma once
#include "states/IGameState.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class PauseState : public IGameState {
public:
    // 1. Constructor / Destructor
    /// Loads the standard UI font; degrades to a text-free modal when the
    /// asset is missing (e.g. incomplete deployment) instead of crashing.
    PauseState();
    /// Build the state against an explicit font path so
    /// the missing-font degradation path is exercisable on every platform.
    explicit PauseState(const std::string& fontPath);
    ~PauseState() override = default;

    // 2. Override methods
    void onEnter() override;
    void onExit() override;
    void processEvents(const sf::Event& event) override;
    void processInput(const InputState& inputState) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

    bool isOverlay() const override { return true; }

    /// Whether the interactive volume/selection menu items exist. False when
    /// the UI font failed to load: mouse hit-testing is then disabled and
    /// only the keyboard resume path stays active.
    bool hasInteractiveMenu() const { return m_menuTexts.size() >= 4; }

private:
    void initLayout();
    void refreshVolumeDisplay();
    void adjustVolume(int itemIndex, float delta);
    void executeSelection(int index);

    // 6. Private members
    sf::Font m_font;
    bool m_fontLoaded = false;

    std::optional<sf::Text> m_titleText;
    std::optional<sf::Text> m_subtitleText;
    std::optional<sf::Text> m_hintText;

    // 4 Menu items:
    // 0: MUSIC VOLUME
    // 1: SOUND VOLUME
    // 2: RESUME GAME
    // 3: QUIT TO MAIN MENU
    std::vector<sf::Text> m_menuTexts;
    int m_selectedIndex = 0;

    sf::RectangleShape m_panel;
    sf::RectangleShape m_highlightBar;
    float m_animTimer = 0.f;
};
