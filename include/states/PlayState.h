/**
 * @file PlayState.h
 * @author TV1 (Dương)
 * @brief Play state implementation (handles the main gameplay loop)
 * @note Sprint 6 — S6-TV1-07/08/09/10/11/12/13: one-based level progression,
 *       session GameProgress, load error handling, transition freeze,
 *       terminal-result race guard.
 */

#pragma once

#include "states/IGameState.h"
#include "level/Level.h"
#include "patterns/InputHandler.h"
#include "ui/HUD.h"
#include "patterns/IObserver.h"
#include "core/GameProgress.h"
#include "core/LevelCatalog.h"

class PlayState : public IGameState, public IObserver {
public:
    // 1. Constructor / Destructor
    PlayState();
    ~PlayState() override;

    // 2. Override methods
    void onEnter() override;
    void onExit() override;
    void onPause() override;
    void onResume() override;

    void processEvents(const sf::Event& event) override;
    void processInput(const InputState& inputState) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

    void onNotify(EventType event) override;

private:
    void rebindCommands();

    /// Load a level by one-based number and restore session progress onto Mario/HUD.
    /// Returns false if the level file is missing/invalid.
    bool loadLevel(int levelNumber);

    /// Re-load the current level and restore progress (used after death/retry/next-level).
    /// Returns false if the level file is missing/invalid (a Menu transition is queued).
    bool navigateToLevel(int levelNumber);

    /// Snapshot current Mario/HUD values into m_progress (called before reload/destroy).
    void snapshotProgress();
    /// Apply m_progress values to the freshly created Mario/HUD.
    void restoreProgress();

    /// S6-TV1-12: advance the transition state machine (freeze gameplay during it).
    void updateTransition(float dt);

    // 6. Private members
    std::unique_ptr<Level> m_level;
    InputHandler m_inputHandler;
    std::unique_ptr<HUD> m_hud;

    GameProgress m_progress;   ///< Session progress, independent of Level lifetime.

    sf::RectangleShape m_fadeOverlay;
    float m_fadeAlpha = 0.f;
    float m_fadeDuration = 0.5f;

    bool m_needsReload = false;
    bool m_needsGameOver = false;

    float m_deathDelayTimer = 0.f;
    bool m_isGameOverPending = false;
    bool m_isReloadPending = false;
    /// S6-TV1-13: only one terminal result can be committed per frame.
    bool m_terminalCommittedThisFrame = false;

    /// S6-TV1-12: transition state machine
    enum class TransitionPhase { NONE, FADE_OUT, LOADING, FADE_IN };
    TransitionPhase m_transitionPhase = TransitionPhase::NONE;
    int m_transitionTargetLevel = 0;
    bool m_transitionIsWin = false;
    bool m_skipNextDelta = false; ///< S6-TV2-21: ignore dt spike after loading
};