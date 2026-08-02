/**
 * @file MoveLeftCommand.cpp
 * @author TV5
 * @brief Concrete Command — triggers Mario to move left
 * @note Week 2 — bound to sf::Keyboard::Key::Left by default in InputHandler
 */

#include "patterns/MoveLeftCommand.h"
#include "entities/Mario.h"

// ============================================================
// PATTERN: Command (concrete)
// Reason: encapsulates "move left" as an object; InputHandler
//         can rebind this action to any key at runtime
// ============================================================

MoveLeftCommand::MoveLeftCommand(Mario* mario) : m_mario(mario) {}

void MoveLeftCommand::execute() {
    if (m_mario) {
        m_mario->moveLeft();
    }
}

void MoveLeftCommand::undo() {
    if (m_mario) {
        m_mario->stopMoving();
    }
}
