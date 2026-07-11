/**
 * @file JumpCommand.h
 * @author TV5
 * @brief Concrete Command — triggers Mario to jump
 * @note Week 2 — bound to sf::Keyboard::Key::Space by default in InputHandler
 */

#pragma once

#include "patterns/ICommand.h"

class Mario;

// ============================================================
// PATTERN: Command (concrete)
// Reason: encapsulates "jump" as an object; InputHandler
//         can rebind this action to any key at runtime
// ============================================================

class JumpCommand : public ICommand {
public:
    // 1. Constructor / Destructor
    explicit JumpCommand(Mario* mario);
    ~JumpCommand() override = default;

    // 2. Override methods
    void execute() override;
    void undo() override;

private:
    // 6. Private members
    Mario* m_mario;  // non-owning pointer
};
