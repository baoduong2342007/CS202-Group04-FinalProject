/**
 * @file PlayState.h
 * @author TV1 (Dương)
 * @brief Play state implementation (handles the main gameplay loop)
 */

#pragma once

#include "states/IGameState.h"
#include "level/Level.h"
#include "patterns/InputHandler.h"
#include "ui/HUD.h"
#include "patterns/IObserver.h"

class PlayState : public IGameState, public IObserver {
public:
    // 1. Constructor / Destructor
    PlayState();
    ~PlayState() override;

    // 2. Override methods
    void onEnter() override;
    void onExit() override;
    
    void processEvents(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    void onNotify(EventType event) override;

private:
    void rebindCommands();

    // 6. Private members
    std::unique_ptr<Level> m_level;
    InputHandler m_inputHandler;
    std::unique_ptr<HUD> m_hud;

    sf::RectangleShape m_fadeOverlay;
    float m_fadeAlpha = 0.f;
    bool m_isFading = false;
    float m_fadeDuration = 0.5f;

    int m_currentLevel = 1;
    static constexpr int MAX_LEVELS = 2;
    std::string getCurrentLevelPath() const;
};
