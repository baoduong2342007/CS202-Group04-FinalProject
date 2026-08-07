/**
 * @file WinState.h
 * @author TV1 (Dương)
 * @brief Win State
 */
#pragma once
#include "states/IGameState.h"
#include "core/GameProgress.h"
#include <SFML/Graphics.hpp>

class WinState : public IGameState {
public:
    explicit WinState(const GameProgress& progress = {});
    ~WinState() override = default;

    void onEnter() override;
    void onExit() override;
    void processEvents(const sf::Event& event) override;
    void processInput(const InputState& inputState) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    sf::Font m_font;
    sf::Text m_text;
    GameProgress m_progress;
    sf::Text m_scoreText;
};
