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
#include "patterns/RunCommand.h"
#include "patterns/ShootCommand.h"

#include "states/GameOverState.h"
#include "states/WinState.h"
#include "states/PauseState.h"
#include "states/MenuState.h"
#include <algorithm>
#include <cstdint>
#include <iostream>

#include "core/GameManager.h"
#include "core/SoundManager.h"

namespace {
    const sf::Color FADE_START_COLOR(0, 0, 0, 0);

    // Death/Damage Camera Shake configuration
    constexpr float DEATH_SHAKE_DURATION = 0.5f;
    constexpr float DEATH_SHAKE_INTENSITY = 15.0f;
    constexpr float DAMAGE_SHAKE_DURATION = 0.3f;
    constexpr float DAMAGE_SHAKE_INTENSITY = 8.0f;
    constexpr float DEATH_ANIMATION_FALLBACK_TIMEOUT = 1.0f;
}

PlayState::PlayState(int startLevel, CharacterType characterType) {
    m_progress.currentLevel = std::clamp(startLevel, 1, std::max(1, LevelCatalog::count()));
    m_progress.character = characterType;
    m_fadeOverlay.setFillColor(FADE_START_COLOR);
}

PlayState::PlayState(CharacterType characterType)
    : PlayState(1, characterType) {}

PlayState::PlayState(int startLevel, CharacterType playerOne, CharacterType playerTwo) {
    m_progress.currentLevel = std::clamp(startLevel, 1, std::max(1, LevelCatalog::count()));
    m_progress.character = playerOne;
    m_progress.character2 = playerTwo;
    m_progress.isCoop = true;
    m_isCoop = true;
    m_fadeOverlay.setFillColor(FADE_START_COLOR);
}

PlayState::~PlayState() {
    EventBus::getInstance().unsubscribe(EventType::PLAYER_DIED, this);
    EventBus::getInstance().unsubscribe(EventType::PLAYER_POWER_DOWN, this);
    EventBus::getInstance().unsubscribe(EventType::LEVEL_COMPLETED, this);
    EventBus::getInstance().unsubscribe(EventType::GAME_PAUSED, this);
}

void PlayState::rebindCommands() {
    m_inputHandler.clear(); // Reset handlers
    m_inputHandler2.clear();

    if (m_isCoop) {
        rebindCoopCommands();
        return;
    }

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

        const auto requestUp = [this] {
            if (m_level && m_level->getMario()) m_level->getMario()->setVerticalIntent(-1.0f);
        };
        const auto requestDown = [this] {
            if (m_level && m_level->getMario()) m_level->getMario()->setVerticalIntent(1.0f);
        };
        m_inputHandler.bindKey(sf::Keyboard::Key::W, std::make_unique<RunCommand>(requestUp), InputTrigger::Held, InputGroup::Vertical);
        m_inputHandler.bindKey(sf::Keyboard::Key::Up, std::make_unique<RunCommand>(requestUp), InputTrigger::Held, InputGroup::Vertical);
        m_inputHandler.bindKey(sf::Keyboard::Key::S, std::make_unique<RunCommand>(requestDown), InputTrigger::Held, InputGroup::Vertical);
        m_inputHandler.bindKey(sf::Keyboard::Key::Down, std::make_unique<RunCommand>(requestDown), InputTrigger::Held, InputGroup::Vertical);

        const auto requestRun = [this] {
            if (m_level && m_level->getMario()) {
                m_level->getMario()->setRunIntent(true);
            }
        };
        m_inputHandler.bindKey(sf::Keyboard::Key::LShift,
                               std::make_unique<RunCommand>(requestRun),
                               InputTrigger::Held);
        m_inputHandler.bindKey(sf::Keyboard::Key::RShift,
                               std::make_unique<RunCommand>(requestRun),
                               InputTrigger::Held);

        m_inputHandler.bindKey(sf::Keyboard::Key::X,
                               std::make_unique<ShootCommand>([this] {
                                   if (m_level && m_level->getMario()) {
                                       m_level->requestFireBallShot(*m_level->getMario());
                                   }
                               }),
                               InputTrigger::Pressed);
        m_inputHandler.bindKey(sf::Keyboard::Key::Escape,
                               std::make_unique<PauseCommand>(),
                               InputTrigger::Pressed);
    }
}

void PlayState::rebindCoopCommands() {
    Mario* p1 = m_level ? m_level->getMario() : nullptr;
    Mario* p2 = m_level ? m_level->getMario2() : nullptr;

    // Player one: A/D move, W jump, S pipe-down, X shoot, LShift run.
    if (p1) {
        m_inputHandler.bindKey(sf::Keyboard::Key::A,
                               std::make_unique<MoveLeftCommand>(p1),
                               InputTrigger::Held,
                               InputGroup::Horizontal);
        m_inputHandler.bindKey(sf::Keyboard::Key::D,
                               std::make_unique<MoveRightCommand>(p1),
                               InputTrigger::Held,
                               InputGroup::Horizontal);
        m_inputHandler.bindKey(sf::Keyboard::Key::W,
                               std::make_unique<JumpCommand>(p1),
                               InputTrigger::Pressed);

        const auto requestUp = [p1] {
            if (p1) p1->setVerticalIntent(-1.0f);
        };
        const auto requestDown = [p1] {
            if (p1) p1->setVerticalIntent(1.0f);
        };
        m_inputHandler.bindKey(sf::Keyboard::Key::W, std::make_unique<RunCommand>(requestUp), InputTrigger::Held, InputGroup::Vertical);
        m_inputHandler.bindKey(sf::Keyboard::Key::S, std::make_unique<RunCommand>(requestDown), InputTrigger::Held, InputGroup::Vertical);

        const auto requestRun = [p1] {
            if (p1) p1->setRunIntent(true);
        };
        m_inputHandler.bindKey(sf::Keyboard::Key::LShift,
                               std::make_unique<RunCommand>(requestRun),
                               InputTrigger::Held);

        m_inputHandler.bindKey(sf::Keyboard::Key::X,
                               std::make_unique<ShootCommand>([this, p1] {
                                   if (m_level && p1) {
                                       m_level->requestFireBallShot(*p1);
                                   }
                               }),
                               InputTrigger::Pressed);
    }

    // Player two: arrows move, Up jump, Down pipe-down, '/' shoot, RShift run.
    if (p2) {
        m_inputHandler2.bindKey(sf::Keyboard::Key::Left,
                                std::make_unique<MoveLeftCommand>(p2),
                                InputTrigger::Held,
                                InputGroup::Horizontal);
        m_inputHandler2.bindKey(sf::Keyboard::Key::Right,
                                std::make_unique<MoveRightCommand>(p2),
                                InputTrigger::Held,
                                InputGroup::Horizontal);
        m_inputHandler2.bindKey(sf::Keyboard::Key::Up,
                                std::make_unique<JumpCommand>(p2),
                                InputTrigger::Pressed);

        const auto requestUp = [p2] {
            if (p2) p2->setVerticalIntent(-1.0f);
        };
        const auto requestDown = [p2] {
            if (p2) p2->setVerticalIntent(1.0f);
        };
        m_inputHandler2.bindKey(sf::Keyboard::Key::Up, std::make_unique<RunCommand>(requestUp), InputTrigger::Held, InputGroup::Vertical);
        m_inputHandler2.bindKey(sf::Keyboard::Key::Down, std::make_unique<RunCommand>(requestDown), InputTrigger::Held, InputGroup::Vertical);

        const auto requestRun = [p2] {
            if (p2) p2->setRunIntent(true);
        };
        m_inputHandler2.bindKey(sf::Keyboard::Key::RShift,
                                std::make_unique<RunCommand>(requestRun),
                                InputTrigger::Held);

        m_inputHandler2.bindKey(sf::Keyboard::Key::Slash,
                                std::make_unique<ShootCommand>([this, p2] {
                                    if (m_level && p2) {
                                        m_level->requestFireBallShot(*p2);
                                    }
                                }),
                                InputTrigger::Pressed);
    }

    // Pause stays a shared team action on player one's handler.
    m_inputHandler.bindKey(sf::Keyboard::Key::Escape,
                           std::make_unique<PauseCommand>(),
                           InputTrigger::Pressed);
}

void PlayState::onEnter() {
    EventBus::getInstance().subscribe(EventType::PLAYER_DIED, this);
    EventBus::getInstance().subscribe(EventType::PLAYER_POWER_DOWN, this);
    EventBus::getInstance().subscribe(EventType::LEVEL_COMPLETED, this);
    EventBus::getInstance().subscribe(EventType::GAME_PAUSED, this);

    // S6-TV1-11: load the initial level here, not in the constructor, so a failure
    // can propagate a Menu transition in the correct order. On success we emit
    // LEVEL_STARTED exactly once.
    if (!loadLevel(m_progress.currentLevel)) {
        std::cerr << "[PlayState] Failed to load Level " << m_progress.currentLevel
                  << " — returning to Menu." << std::endl;
        GameManager::getInstance().changeState(std::make_unique<MenuState>());
        return;
    }

    restoreProgress(); // no-op for default progress on a brand-new Level 1
    rebindCommands();
    // Music is already started by loadLevel() -> playMusic(def->music).
    // A second playMusic() here would restart the stream from the beginning.
    EventBus::getInstance().notify(EventType::LEVEL_STARTED);
}

void PlayState::onExit() {
    EventBus::getInstance().unsubscribe(EventType::PLAYER_DIED, this);
    EventBus::getInstance().unsubscribe(EventType::PLAYER_POWER_DOWN, this);
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
    EventBus::getInstance().notify(EventType::GAME_RESUMED);
}

void PlayState::onNotify(EventType event) {
    // S6-TV1-13: reject terminal events if one was already committed this frame.
    if (m_terminalCommittedThisFrame) {
        return;
    }

    if (event == EventType::PLAYER_DIED) {
        if (m_level) {
            m_level->getCamera().shake(DEATH_SHAKE_DURATION, DEATH_SHAKE_INTENSITY);
        }
        m_terminalCommittedThisFrame = true;
        // Mario's animation completion is authoritative. This longer timer is
        // only a fail-safe for a missing/corrupt animation asset.
        m_deathDelayTimer = DEATH_ANIMATION_FALLBACK_TIMEOUT;
        // S6-TV1-19: commit the current score to the high score the moment a
        // death happens — dying with lives left must STILL persist the session
        // score. GameOver/Win keep calling updateHighScore (monotonic) as a
        // harmless fallback.
        if (m_level && m_level->getMario()) {
            int sessionScore = m_level->getMario()->getScore();
            int teamLives = m_level->getMario()->getLives();
            if (m_isCoop && m_level->getMario2()) {
                // Team totals: only the dying player's lives were just
                // decremented, so the minimum is the shared pool.
                sessionScore += m_level->getMario2()->getScore();
                teamLives = std::min(teamLives, m_level->getMario2()->getLives());
            }
            GameManager::getInstance().getSaveManager()
                .updateHighScore(sessionScore);
            if (teamLives > 0) {
                m_isReloadPending = true;
            } else {
                m_isGameOverPending = true;
            }
        } else {
            m_isGameOverPending = true;
        }
    } else if (event == EventType::PLAYER_POWER_DOWN) {
        if (m_level) {
            m_level->getCamera().shake(DAMAGE_SHAKE_DURATION, DAMAGE_SHAKE_INTENSITY);
        }
    } else if (event == EventType::LEVEL_COMPLETED) {
        m_terminalCommittedThisFrame = true;
        // S6-TV1-09: snapshot BEFORE destroying the level so we never read
        // data from a Mario that is about to be destroyed.
        snapshotProgress();
        m_progress.currentLevel++;
        GameManager::getInstance().getSaveManager().updateHighestUnlockedLevel(
            std::min(m_progress.currentLevel, LevelCatalog::count()));
        // S6-TV1-19: persist the high score at level completion too, so reaching
        // the finish right before quitting is never lost.
        GameManager::getInstance().getSaveManager().updateHighScore(m_progress.score);
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
    if (m_isCoop) {
        processCoopInput(inputState);
        return;
    }

    if (m_level && m_level->getMario()) {
        // Run is an edge-free per-frame intent. Resetting it before dispatch
        // prevents Shift held during a transition/pause from being buffered.
        m_level->getMario()->setRunIntent(false);
    }

    // S6-TV1-12: block all gameplay input during a transition (freeze).
    if (m_transitionPhase != TransitionPhase::NONE) {
        return;
    }
    // S6-TV5-05: block commands when death/GameOver pending or player inactive.
    if (!m_level || !m_level->getMario() ||
        m_needsReload || m_needsGameOver ||
        !m_level->getMario()->isActive() ||
        m_level->getMario()->isDying() ||
        m_level->getMario()->isTransforming() ||
        m_level->getMario()->isFlagpoleSliding() ||
        m_level->isFlagSequenceActive()) {
        return;
    }

    m_level->getMario()->setMoveIntent(0.0f);
    m_level->getMario()->setVerticalIntent(0.0f);

    const bool isShiftHeld = inputState.isHeld(sf::Keyboard::Key::LShift) ||
                             inputState.isHeld(sf::Keyboard::Key::RShift) ||
                             inputState.isHeld(sf::Keyboard::Key::J);
    m_level->getMario()->setRunning(isShiftHeld);

    m_inputHandler.handleInput(inputState);

    if (inputState.wasReleased(sf::Keyboard::Key::W) ||
        inputState.wasReleased(sf::Keyboard::Key::Up) ||
        inputState.wasReleased(sf::Keyboard::Key::Space)) {
        m_level->getMario()->releaseJump();
    }
}

void PlayState::processCoopInput(const InputState& inputState) {
    if (!m_level) {
        return;
    }

    Mario* p1 = m_level->getMario();
    Mario* p2 = m_level->getMario2();

    // S6-TV1-12: block all gameplay input during a transition (freeze), and
    // while a terminal result or the shared flagpole cinematic is running.
    if (m_transitionPhase != TransitionPhase::NONE ||
        m_needsReload || m_needsGameOver ||
        m_level->isFlagSequenceActive()) {
        return;
    }

    const auto dispatchPlayer = [&inputState](
        Mario* player, InputHandler& handler,
        sf::Keyboard::Key runKey, sf::Keyboard::Key jumpKey) {
        // Reset per-frame intents first so held keys cannot bleed through a
        // pause or a scripted sequence.
        player->setMoveIntent(0.0f);
        player->setVerticalIntent(0.0f);
        player->setRunIntent(false);

        if (!player->isActive() || player->isDying() ||
            player->isTransforming() || player->isFlagpoleSliding()) {
            return;
        }

        if (inputState.isHeld(runKey)) {
            player->setRunIntent(true);
        }
        handler.handleInput(inputState);

        if (inputState.wasReleased(jumpKey)) {
            player->releaseJump();
        }
    };

    if (p1) {
        dispatchPlayer(p1, m_inputHandler,
                       sf::Keyboard::Key::LShift, sf::Keyboard::Key::W);
    }
    if (p2) {
        dispatchPlayer(p2, m_inputHandler2,
                       sf::Keyboard::Key::RShift, sf::Keyboard::Key::Up);
    }
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
    m_progress.character = m_level->getMario()->getCharacterType();

    // Co-op team accounting: score/coins are the team total, lives are the
    // shared pool (equal on both players except the one who just died, so
    // the minimum is exactly the post-death team value).
    if (m_isCoop && m_level->getMario2()) {
        const Mario* playerTwo = m_level->getMario2();
        m_progress.score += playerTwo->getScore();
        m_progress.coins += playerTwo->getCoinCount();
        m_progress.lives = std::min(m_progress.lives, playerTwo->getLives());
    }
}

void PlayState::restoreProgress() {
    if (!m_level || !m_level->getMario()) {
        return;
    }

    // S6-TV1-10: apply session progress to the fresh Mario/HUD.
    m_level->getMario()->setCharacterType(m_progress.character);
    m_level->getMario()->setScore(m_progress.score);
    m_level->getMario()->setCoinCount(m_progress.coins);
    m_level->getMario()->setLives(m_progress.lives);
    if (m_progress.power != MarioState::SMALL) {
        m_level->getMario()->setMarioState(m_progress.power);
    }

    // Co-op: the team totals land on player one so the invariant
    // "team score == p1.score + p2.score" holds across reloads; player two
    // restarts each level SMALL and re-accumulates his own pickups.
    if (m_isCoop && m_level->getMario2()) {
        Mario* playerTwo = m_level->getMario2();
        playerTwo->setCharacterType(m_progress.character2);
        playerTwo->setScore(0);
        playerTwo->setCoinCount(0);
        playerTwo->setLives(m_progress.lives);
    }

    if (m_hud) {
        m_hud->setWorldLevel(1, m_progress.currentLevel);
        m_hud->update();
    }

    // Synchronize initial sprite transforms and camera frame 0 with restored MarioState
    m_level->update(0.f);
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
    m_level->setTheme(def->theme);
    m_level->setCameraVerticalMode(def->cameraMode);
    const bool levelLoaded = m_isCoop
        ? m_level->loadFromFile(def->filePath, m_progress.character, m_progress.character2)
        : m_level->loadFromFile(def->filePath, m_progress.character);
    if (!levelLoaded) {
        m_level.reset();
        return false;
    }

    // Create HUD after Level (and Mario) are initialized
    if (m_level->getMario()) {
        m_hud = std::make_unique<HUD>(*(m_level->getMario()), 1, def->number);
        if (m_isCoop && m_level->getMario2()) {
            m_hud->attachSecondPlayer(*(m_level->getMario2()));
        }
        m_hud->setTimeWarningCallback([] {
            SoundManager::getInstance().playSound("hurryup");
        });
        m_hud->setTimeoutCallback([this] {
            if (!m_level || !m_level->getMario() ||
                m_transitionPhase != TransitionPhase::NONE ||
                !m_level->getMario()->isActive() ||
                m_level->getMario()->isDying()) {
                return;
            }
            // HUD owns the one-shot timeout edge; Mario owns the death event
            // and life decrement, keeping timeout/death behavior centralized.
            m_level->getMario()->loseLife();
        });
    }

    // LevelCatalog owns the level-to-MusicId mapping; SoundManager owns the
    // path resolution and streaming lifecycle.
    SoundManager::getInstance().setLevelMusic(def->music);
    SoundManager::getInstance().playMusic(def->music);

    m_fadeOverlay.setFillColor(FADE_START_COLOR);
    return true;
}

bool PlayState::navigateToLevel(int levelNumber) {
    // S6-TV1-10/12: restore progress onto the newly created Mario/HUD.
    if (!loadLevel(levelNumber)) {
        std::cerr << "[PlayState] Failed to load Level " << levelNumber
                  << " — returning to Menu." << std::endl;
        GameManager::getInstance().changeState(std::make_unique<MenuState>());
        return false;
    }
    restoreProgress();
    rebindCommands();
    m_fadeAlpha = 0.f;
    return true;
}

void PlayState::update(float dt) {
    // S6-TV1-13: only one terminal result (GameOver or level transition)
    // may be committed per frame.
    m_terminalCommittedThisFrame = false;

    // S6-TV1-12: freeze gameplay during transition (fade out → load → fade in).
    if (m_transitionPhase != TransitionPhase::NONE) {
        updateTransition(dt);
        if (m_hud) {
            m_hud->update(dt, false);
        }
        return;
    }

    if (m_isGameOverPending || m_isReloadPending) {
        m_deathDelayTimer = std::max(0.f, m_deathDelayTimer - dt);
        // In co-op either player's finished death animation releases the
        // pending result; the fallback timer still guards against a missing
        // animation asset.
        const auto dyingAnimationFinished = [](const Mario* player) {
            return player && player->isDying() && player->isDeathAnimationFinished();
        };
        const bool animationFinished =
            m_level &&
            (dyingAnimationFinished(m_level->getMario()) ||
             dyingAnimationFinished(m_level->getMario2()));
        const bool fallbackExpired = m_deathDelayTimer <= 0.f;
        if (animationFinished || fallbackExpired) {
            if (m_isGameOverPending) {
                m_needsGameOver = true;
            } else if (m_isReloadPending) {
                m_needsReload = true;
            }
            m_isGameOverPending = false;
            m_isReloadPending = false;
        }
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
        snapshotProgress(); // CRITICAL: Save progress (like decremented lives) before reloading!
        navigateToLevel(m_progress.currentLevel);
        return;
    }

    if (!m_level) {
        return; // Level failed to load — Menu transition is pending.
    }

    // Update level entities
    m_level->update(dt);

    if (m_hud) {
        const bool gameplayActive = m_transitionPhase == TransitionPhase::NONE &&
                                     !m_needsReload && !m_needsGameOver;
        m_hud->update(dt, gameplayActive);
    }
}

void PlayState::updateTransition(float dt) {
    if (m_skipNextDelta) {
        dt = 0.f;
        m_skipNextDelta = false;
    }

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
            } else if (!navigateToLevel(m_transitionTargetLevel)) {
                // S6-TV1-11/12: the reload failed — a Menu transition has been
                // queued. Abort the transition WITHOUT entering FADE_IN and WITHOUT
                // emitting LEVEL_STARTED for a level that never loaded.
                m_transitionPhase = TransitionPhase::NONE;
            } else {
                // navigateToLevel() resets m_fadeAlpha to 0 — restore it so
                // FADE_IN starts from a fully black screen.
                m_fadeAlpha = 255.f;
                m_fadeOverlay.setFillColor(sf::Color(0, 0, 0, 255));
                m_transitionPhase = TransitionPhase::FADE_IN;
                m_skipNextDelta = true; // S6-TV2-21: skip lag spike on next frame
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

void PlayState::render(sf::RenderTarget& target) {
    if (!m_level) {
        return; // Level failed to load — Menu transition is pending.
    }

    m_level->render(target);

    // Switch to default view for UI overlay
    target.setView(target.getDefaultView());
    if (m_hud) {
        m_hud->draw(target);
    }

    if (m_fadeAlpha > 0.f) {
        m_fadeOverlay.setSize(sf::Vector2f(target.getSize()));
        target.draw(m_fadeOverlay);
    }
}
