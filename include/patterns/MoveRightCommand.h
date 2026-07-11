/**
 * @file MoveRightCommand.h
 * @author TV5
 * @brief Concrete Command — triggers Mario to move right
 * @note Week 2 — bound to sf::Keyboard::Key::Right by default in InputHandler
 */

#ifndef MOVERIGHTCOMMAND_H
#define MOVERIGHTCOMMAND_H

#include "patterns/ICommand.h"

class Mario;

// ============================================================
// PATTERN: Command (concrete)
// Reason: encapsulates "move right" as an object; InputHandler
//         can rebind this action to any key at runtime
// ============================================================

class MoveRightCommand : public ICommand {
public:
    // 1. Constructor / Destructor
    explicit MoveRightCommand(Mario* mario);
    ~MoveRightCommand() override = default;

    // 2. Override methods
    void execute() override;
    void undo() override;

private:
    // 6. Private members
    Mario* m_mario;  // non-owning pointer
};

#endif // MOVERIGHTCOMMAND_H
