/**
 * @file MoveRightCommand.cpp
 * @author TV5
 * @brief Concrete Command — triggers Mario to move right
 * @note Week 2 — bound to sf::Keyboard::Key::Right by default in InputHandler
 */

#include "patterns/MoveRightCommand.h"
#include "entities/Mario.h"

// ============================================================
// PATTERN: Command (concrete)
// Reason: encapsulates "move right" as an object; InputHandler
//         can rebind this action to any key at runtime
// ============================================================

MoveRightCommand::MoveRightCommand(Mario* mario) : m_mario(mario) {}

void MoveRightCommand::execute() {
    if (m_mario) {
        m_mario->moveRight();
    }
}
