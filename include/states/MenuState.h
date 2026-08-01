/**
 * @file MenuState.h
 * @author TV1 (Dương)
 * @brief Main Menu State
 */
#pragma once
#include "states/IGameState.h"
#include <SFML/Graphics.hpp>

class MenuState : public IGameState {
public:
    MenuState();
    ~MenuState() override = default;

    void onEnter() override;
    void onExit() override;
    void processEvents(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    sf::Font m_font;
    sf::Text m_text;
};
