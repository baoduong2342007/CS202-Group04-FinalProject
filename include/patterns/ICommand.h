/**
 * @file ICommand.h
 * @author TV5
 * @brief Command pattern interface — encapsulates a request as an object
 * @note Week 2 — used by InputHandler to map keys to game actions
 */

#ifndef ICOMMAND_H
#define ICOMMAND_H

// ============================================================
// PATTERN: Command
// Reason: decouples input handling from game actions;
//         allows remapping keys without modifying game logic
// ============================================================

class ICommand {
public:
    virtual ~ICommand() = default;

    virtual void execute() = 0;

    virtual void undo() = 0;
};

#endif // ICOMMAND_H
