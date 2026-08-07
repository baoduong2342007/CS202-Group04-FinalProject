/**
 * @file GameOverState.h
 * @author TV1 (Dương)
 * @brief Game Over State
 */
#pragma once
#include "states/IGameState.h"
#include "core/GameProgress.h"
#include <SFML/Graphics.hpp>

class GameOverState : public IGameState {
public:
    explicit GameOverState(const GameProgress& progress = {});
    ~GameOverState() override = default;

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
