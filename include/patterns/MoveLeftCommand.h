/**
 * @file MoveLeftCommand.h
 * @author TV5
 * @brief Concrete Command — triggers Mario to move left
 * @note Week 2 — bound to sf::Keyboard::Key::Left by default in InputHandler
 */

#ifndef MOVELEFTCOMMAND_H
#define MOVELEFTCOMMAND_H

#include "patterns/ICommand.h"

class Mario;

// ============================================================
// PATTERN: Command (concrete)
// Reason: encapsulates "move left" as an object; InputHandler
//         can rebind this action to any key at runtime
// ============================================================

class MoveLeftCommand : public ICommand {
public:
    // 1. Constructor / Destructor
    explicit MoveLeftCommand(Mario* mario);
    ~MoveLeftCommand() override = default;

    // 2. Override methods
    void execute() override;
    void undo() override;

private:
    // 6. Private members
    Mario* m_mario;  // non-owning pointer — Command does not own Mario
};

#endif // MOVELEFTCOMMAND_H
