/**
 * @file JumpCommand.cpp
 * @author TV5
 * @brief Concrete Command — triggers Mario to jump
 * @note Week 2 — bound to sf::Keyboard::Key::Space by default in InputHandler
 */

#include "patterns/JumpCommand.h"
#include "entities/Mario.h"

// ============================================================
// PATTERN: Command (concrete)
// Reason: encapsulates "jump" as an object; InputHandler
//         can rebind this action to any key at runtime
// ============================================================

JumpCommand::JumpCommand(Mario* mario) : m_mario(mario) {}

void JumpCommand::execute() {
    if (m_mario) {
        m_mario->jump();
    }
}
