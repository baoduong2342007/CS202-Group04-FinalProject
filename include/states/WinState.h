/**
 * @file WinState.h
 * @author TV1 (Dương)
 * @brief Win State
 */
#pragma once
#include "states/IGameState.h"
#include <SFML/Graphics.hpp>

class WinState : public IGameState {
public:
    WinState();
    ~WinState() override = default;

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
