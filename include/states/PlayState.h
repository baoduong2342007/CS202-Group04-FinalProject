/**
 * @file PlayState.h
 * @author TV1 (Dương)
 * @brief Play state implementation (handles the main gameplay loop)
 */

#pragma once

#include "states/IGameState.h"
#include "level/Level.h"
#include "patterns/InputHandler.h"
#include "physics/PhysicsEngine.h"

class PlayState : public IGameState {
public:
    // 1. Constructor / Destructor
    PlayState();
    ~PlayState() override = default;

    // 2. Override methods
    void onEnter() override;
    void onExit() override;
    
    void processEvents(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    // 6. Private members
    Level m_level;
    InputHandler m_inputHandler;
};
