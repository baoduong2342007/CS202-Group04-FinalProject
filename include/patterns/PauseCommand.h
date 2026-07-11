/**
 * @file PauseCommand.h
 * @author TV5
 * @brief Concrete Command — triggers the game to pause
 * @note Week 2 — bound to sf::Keyboard::Key::Escape by default in InputHandler
 */

#pragma once

#include "patterns/ICommand.h"

class GameManager;

// ============================================================
// PATTERN: Command (concrete)
// Reason: encapsulates "pause" as an object; decoupled from
//         specific key binding so it can be rebound freely
// ============================================================

class PauseCommand : public ICommand {
public:
    // 1. Constructor / Destructor
    explicit PauseCommand(GameManager* gameManager);
    ~PauseCommand() override = default;

    // 2. Override methods
    void execute() override;
    void undo() override;

private:
    // 6. Private members
    GameManager* m_gameManager;  // non-owning pointer — delegates to state machine
};
