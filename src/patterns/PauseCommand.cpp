/**
 * @file PauseCommand.cpp
 * @author TV5
 * @brief Concrete Command — triggers the game to pause
 * @note Week 2 — bound to sf::Keyboard::Key::Escape by default in InputHandler
 */

#include "patterns/PauseCommand.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"

// ============================================================
// PATTERN: Command (concrete)
// Reason: encapsulates "pause" as an object; decoupled from
//         specific key binding so it can be rebound freely
// ============================================================

PauseCommand::PauseCommand(GameManager* gameManager)
    : m_gameManager(gameManager) {}

void PauseCommand::execute() {
    // Broadcast pause event via EventBus — any subscriber
    // (GameManager, HUD, SoundManager) can react independently
    EventBus::getInstance().notify(EventType::GAME_PAUSED);
}

void PauseCommand::undo() {
    // Resume is handled by the state machine (PauseState → PlayState),
    // not by reversing this command — intentional no-op
}
