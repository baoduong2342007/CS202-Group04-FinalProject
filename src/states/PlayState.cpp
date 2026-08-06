/**
 * @file PlayState.cpp
 * @author TV1 (Dương)
 * @brief Play state implementation (handles the main gameplay loop)
 */

#include "states/PlayState.h"

#include "patterns/EventBus.h"

#include "patterns/JumpCommand.h"
#include "patterns/MoveLeftCommand.h"
#include "patterns/MoveRightCommand.h"
#include "patterns/PauseCommand.h"

#include "states/GameOverState.h"
#include "states/WinState.h"
#include "states/PauseState.h"
#include "states/MenuState.h"
#include "core/GameManager.h"
#include "core/SoundManager.h"
#include <cstdint>
#include <iostream>

namespace {
    const sf::Color FADE_START_COLOR(0, 0, 0, 0);
}

PlayState::PlayState() {
    // S6-TV1-06/07: New Game always starts at Level 1 (one-based), never Level 0.
    m_progress.currentLevel = 1;

    if (!loadLevel(m_progress.currentLevel)) {
        // S6-TV1-11: level load failure returns the player to the Menu with a message.
        std::cerr << "[PlayState] Failed to load Level " << m_progress.currentLevel
                  << " — returning to Menu." << std::endl;
        GameManager::getInstance().changeState(std::make_unique<MenuState>());
        return;
    }

    // Bind commands to InputHandler
    rebindCommands();
    m_fadeOverlay.setFillColor(FADE_START_COLOR);
}

PlayState::~PlayState() {
    EventBus::getInstance().unsubscribe(EventType::PLAYER_DIED, this);
    EventBus::getInstance().unsubscribe(EventType::LEVEL_COMPLETED, this);
    EventBus::getInstance().unsubscribe(EventType::GAME_PAUSED, this);
}

void PlayState::rebindCommands() {
    m_inputHandler.clear(); // Reset handlers

    if (m_level && m_level->getMario()) {
        m_inputHandler.bindKey(sf::Keyboard::Key::A,
                               std::make_unique<MoveLeftCommand>(m_level->getMario()),
                               InputTrigger::Held,
                               InputGroup::Horizontal);
        m_inputHandler.bindKey(sf::Keyboard::Key::Left,
                               std::make_unique<MoveLeftCommand>(m_level->getMario()),
                               InputTrigger::Held,
                               InputGroup::Horizontal);
        
        m_inputHandler.bindKey(sf::Keyboard::Key::D,
                               std::make_unique<MoveRightCommand>(m_level->getMario()),
                               InputTrigger::Held,
                               InputGroup::Horizontal);
        m_inputHandler.bindKey(sf::Keyboard::Key::Right,
                               std::make_unique<MoveRightCommand>(m_level->getMario()),
                               InputTrigger::Held,
                               InputGroup::Horizontal);

        m_inputHandler.bindKey(sf::Keyboard::Key::W,
                               std::make_unique<JumpCommand>(m_level->getMario()),
                               InputTrigger::Pressed);
        m_inputHandler.bindKey(sf::Keyboard::Key::Up,
                               std::make_unique<JumpCommand>(m_level->getMario()),
                               InputTrigger::Pressed);
        m_inputHandler.bindKey(sf::Keyboard::Key::Space,
                               std::make_unique<JumpCommand>(m_level->getMario()),
                               InputTrigger::Pressed);
        m_inputHandler.bindKey(sf::Keyboard::Key::Escape,
                               std::make_unique<PauseCommand>(),
                               InputTrigger::Pressed);
    }
}

void PlayState::onEnter() {
    EventBus::getInstance().subscribe(EventType::PLAYER_DIED, this);
    EventBus::getInstance().subscribe(EventType::LEVEL_COMPLETED, this);
    EventBus::getInstance().subscribe(EventType::GAME_PAUSED, this);
    SoundManager::getInstance().playMusic();
}

void PlayState::onExit() {
    EventBus::getInstance().unsubscribe(EventType::PLAYER_DIED, this);
    EventBus::getInstance().unsubscribe(EventType::LEVEL_COMPLETED, this);
    EventBus::getInstance().unsubscribe(EventType::GAME_PAUSED, this);
    SoundManager::getInstance().stopMusic();
}

void PlayState::onPause() {
    // S6-TV1-15/17: freeze gameplay — pause music; timer/input handled by PauseState overlay.
    SoundManager::getInstance().pauseMusic();
}

void PlayState::onResume() {
    // S6-TV1-15/17: restore music and resume updates.
    SoundManager::getInstance().playMusic();
}

void PlayState::onNotify(EventType event) {
    // S6-TV1-13: reject terminal events if one was already committed this frame.
    if (m_terminalCommittedThisFrame) {
        return;
    }

    if (event == EventType::PLAYER_DIED) {
        m_terminalCommittedThisFrame = true;
        if (m_level && m_level->getMario() && m_level->getMario()->getLives() <= 0) {
            m_needsGameOver = true;
        } else {
            // Defer reload to next frame — may be called during Box2D step
            m_needsReload = true;
        }
    } else if (event == EventType::LEVEL_COMPLETED) {
        m_terminalCommittedThisFrame = true;
        // S6-TV1-09: snapshot BEFORE destroying the level so we never read
        // data from a Mario that is about to be destroyed.
        snapshotProgress();
        m_progress.currentLevel++;
        // S6-TV1-12: start the transition state machine (freeze → fade → load → fade in).
        m_transitionIsWin = LevelCatalog::isPastFinalLevel(m_progress.currentLevel);
        m_transitionTargetLevel = m_progress.currentLevel;
        m_transitionPhase = TransitionPhase::FADE_OUT;
        m_fadeAlpha = 0.f;
    } else if (event == EventType::GAME_PAUSED) {
        GameManager::getInstance().pushState(std::make_unique<PauseState>());
    }
}

void PlayState::processEvents(const sf::Event& event) {
    (void)event;
}

void PlayState::processInput(const InputState& inputState) {
    if (!m_level || !m_level->getMario()) {
        return;
    }

    m_level->getMario()->setMoveIntent(0.0f);
    m_inputHandler.handleInput(inputState);
}

void PlayState::snapshotProgress() {
    if (!m_level || !m_level->getMario()) {
        return;
    }

    // S6-TV1-09: capture exactly once, from the live Mario.
    m_progress.score = m_level->getMario()->getScore();
    m_progress.coins = m_level->getMario()->getCoinCount();
    m_progress.lives = m_level->getMario()->getLives();
    m_progress.power = m_level->getMario()->getMarioState();
}

void PlayState::restoreProgress() {
    if (!m_level || !m_level->getMario()) {
        return;
    }

    // S6-TV1-10: apply session progress to the fresh Mario/HUD.
    m_level->getMario()->setScore(m_progress.score);
    m_level->getMario()->setCoinCount(m_progress.coins);
    m_level->getMario()->setLives(m_progress.lives);
    if (m_progress.power != MarioState::SMALL) {
        m_level->getMario()->setMarioState(m_progress.power);
    }

    if (m_hud) {
        m_hud->setWorldLevel(1, m_progress.currentLevel);
    }
}

bool PlayState::loadLevel(int levelNumber) {
    // S6-TV1-05: resolve the level path from the centralized catalog.
    const LevelDefinition* def = LevelCatalog::find(levelNumber);
    if (!def) {
        std::cerr << "[PlayState] Level " << levelNumber << " not in catalog.\n";
        return false;
    }

    // S6-TV1-11: never ignore the loadFromFile() result.
    m_level = std::make_unique<Level>();
    if (!m_level->loadFromFile(def->filePath)) {
        m_level.reset();
        return false;
    }

    // Create HUD after Level (and Mario) are initialized
    if (m_level->getMario()) {
        m_hud = std::make_unique<HUD>(*(m_level->getMario()), 1, def->number);
    }

    m_fadeOverlay.setFillColor(FADE_START_COLOR);
    return true;
}

void PlayState::navigateToLevel(int levelNumber) {
    // S6-TV1-10/12: restore progress onto the newly created Mario/HUD.
    if (!loadLevel(levelNumber)) {
        std::cerr << "[PlayState] Failed to load Level " << levelNumber
                  << " — returning to Menu." << std::endl;
        GameManager::getInstance().changeState(std::make_unique<MenuState>());
        return;
    }
    restoreProgress();
    rebindCommands();
    m_fadeAlpha = 0.f;
}

void PlayState::update(float dt) {
    // S6-TV1-13: only one terminal result (GameOver or level transition)
    // may be committed per frame.
    m_terminalCommittedThisFrame = false;

    // S6-TV1-12: freeze gameplay during transition (fade out → load → fade in).
    if (m_transitionPhase != TransitionPhase::NONE) {
        updateTransition(dt);
        return;
    }

    // Handle deferred state changes (safe: outside Box2D step)
    if (m_needsGameOver) {
        m_needsGameOver = false;
        m_terminalCommittedThisFrame = true;
        snapshotProgress();
        GameManager::getInstance().changeState(std::make_unique<GameOverState>(m_progress));
        return;
    }
    if (m_needsReload) {
        m_needsReload = false;
        m_terminalCommittedThisFrame = true;
        navigateToLevel(m_progress.currentLevel);
        return;
    }

    if (!m_level) {
        return; // Level failed to load — Menu transition is pending.
    }

    // Update level entities
    m_level->update(dt);

    if (m_hud) {
        m_hud->update();
    }
}

void PlayState::updateTransition(float dt) {
    switch (m_transitionPhase) {
        case TransitionPhase::FADE_OUT:
            m_fadeAlpha += (255.f / m_fadeDuration) * dt;
            if (m_fadeAlpha >= 255.f) {
                m_fadeAlpha = 255.f;
                m_transitionPhase = TransitionPhase::LOADING;
            }
            m_fadeOverlay.setFillColor(sf::Color(0, 0, 0,
                static_cast<std::uint8_t>(m_fadeAlpha)));
            break;

        case TransitionPhase::LOADING:
            if (m_transitionIsWin) {
                GameManager::getInstance().changeState(
                    std::make_unique<WinState>(m_progress));
                m_transitionPhase = TransitionPhase::NONE;
            } else {
                navigateToLevel(m_transitionTargetLevel);
                // navigateToLevel() resets m_fadeAlpha to 0 — restore it so
                // FADE_IN starts from a fully black screen.
                m_fadeAlpha = 255.f;
                m_transitionPhase = TransitionPhase::FADE_IN;
                EventBus::getInstance().notify(EventType::LEVEL_STARTED);
            }
            break;

        case TransitionPhase::FADE_IN:
            m_fadeAlpha -= (255.f / m_fadeDuration) * dt;
            if (m_fadeAlpha <= 0.f) {
                m_fadeAlpha = 0.f;
                m_transitionPhase = TransitionPhase::NONE;
            }
            m_fadeOverlay.setFillColor(sf::Color(0, 0, 0,
                static_cast<std::uint8_t>(m_fadeAlpha)));
            break;

        default:
            break;
    }
}

void PlayState::render(sf::RenderWindow& window) {
    if (!m_level) {
        return; // Level failed to load — Menu transition is pending.
    }

    m_level->render(window);

    // Switch to default view for UI overlay
    window.setView(window.getDefaultView());
    if (m_hud) {
        m_hud->draw(window);
    }

    if (m_fadeAlpha > 0.f) {
        m_fadeOverlay.setSize(sf::Vector2f(window.getSize()));
        window.draw(m_fadeOverlay);
    }
}