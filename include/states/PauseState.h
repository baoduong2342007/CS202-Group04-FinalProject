/**
 * @file PauseState.h
 * @author TV1 (Dương)
 * @brief Pause State
 */
#pragma once
#include "states/IGameState.h"
#include <SFML/Graphics.hpp>

class PauseState : public IGameState {
public:
    PauseState();
    ~PauseState() override = default;

    void onEnter() override;
    void onExit() override;
    void processEvents(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    bool isOverlay() const override { return true; }

private:
    sf::Font m_font;
    sf::Text m_text;
};
