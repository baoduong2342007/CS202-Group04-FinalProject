/**
 * @file PauseCommand.cpp
 * @author TV5
 * @brief Concrete Command — triggers the game to pause
 * @note Week 2 — bound to sf::Keyboard::Key::Escape by default in InputHandler
 * @note Sprint 6 — S6-TV1-24/25: removed undo() and unused GameManager dependency
 */

#include "patterns/PauseCommand.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"

// ============================================================
// PATTERN: Command (concrete)
// Reason: encapsulates "pause" as an object; decoupled from
//         specific key binding so it can be rebound freely
// ============================================================

void PauseCommand::execute() {
    // Broadcast pause event via EventBus — any subscriber
    // (GameManager, HUD, SoundManager) can react independently
    EventBus::getInstance().notify(EventType::GAME_PAUSED);
}