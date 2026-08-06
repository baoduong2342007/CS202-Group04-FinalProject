/**
 * @file GameOverState.h
 * @author TV1 (Dương)
 * @brief Game Over State
 */
#pragma once
#include "states/IGameState.h"
#include <SFML/Graphics.hpp>

class GameOverState : public IGameState {
public:
    GameOverState();
    ~GameOverState() override = default;

    void onEnter() override;
    void onExit() override;
    void processEvents(const sf::Event& event) override;
    void processInput(const InputState& inputState) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    sf::Font m_font;
    sf::Text m_text;
};
