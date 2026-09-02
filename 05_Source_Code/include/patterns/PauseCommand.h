/**
 * @file PauseCommand.h
 * @author TV5
 * @brief Concrete Command — triggers the game to pause
 * @note Week 2 — bound to sf::Keyboard::Key::Escape by default in InputHandler
 * @note Sprint 6 — S6-TV1-25: removed unused GameManager dependency
 */

#pragma once

#include "patterns/ICommand.h"

// ============================================================
// PATTERN: Command (concrete)
// Reason: encapsulates "pause" as an object; decoupled from
//         specific key binding so it can be rebound freely
// ============================================================

class PauseCommand : public ICommand {
public:
    // 1. Constructor / Destructor
    PauseCommand() = default;
    ~PauseCommand() override = default;

    // 2. Override methods
    void execute() override;
};