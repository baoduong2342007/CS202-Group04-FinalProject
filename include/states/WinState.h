/**
 * @file WinState.h
 * @author TV1 (Dương)
 * @brief Win State
 */
#pragma once
#include "states/IGameState.h"
#include "core/GameProgress.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include "ui/UIMenuWidget.h"

class WinState : public IGameState {
public:
    // 1. Constructor / Destructor
    explicit WinState(const GameProgress& progress = {});
    ~WinState() override = default;

    // 2. Override methods
    void onEnter() override;
    void onExit() override;
    void processEvents(const sf::Event& event) override;
    void processInput(const InputState& inputState) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    // 6. Private members
    sf::Font m_font;                             ///< Font for rendering text
    bool m_fontLoaded;                           ///< True if font loaded successfully
    std::optional<sf::Text> m_titleText;         ///< "YOU WIN!" text
    GameProgress m_progress;                     ///< Reference to the current game progress
    std::optional<sf::Text> m_scoreText;         ///< Text displaying the final score
    std::unique_ptr<UIMenuWidget> m_menu;        ///< The UI menu for the state
};
