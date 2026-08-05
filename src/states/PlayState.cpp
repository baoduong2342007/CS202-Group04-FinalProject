/**
 * @file PlayState.cpp
 * @author TV1 (Dương)
 * @brief Play state implementation (handles the main gameplay loop)
 */

#include "states/PlayState.h"

#include "patterns/EventBus.h"

#include "patterns/MoveLeftCommand.h"
#include "patterns/MoveRightCommand.h"

#include "states/GameOverState.h"
#include "states/WinState.h"
#include "states/PauseState.h"
#include "core/GameManager.h"
#include "core/SoundManager.h"
#include <cstdint>

namespace {
    const sf::Color FADE_START_COLOR(0, 0, 0, 0);
}

PlayState::PlayState() {
    m_level = std::make_unique<Level>();
    m_level->loadFromFile(getCurrentLevelPath());

    // Bind commands to InputHandler
    rebindCommands();

    // Create HUD after Level (and Mario) are initialized
    if (m_level->getMario()) {
        m_hud = std::make_unique<HUD>(*(m_level->getMario()));
    }

    m_fadeOverlay.setFillColor(FADE_START_COLOR);
}

PlayState::~PlayState() {
    EventBus::getInstance().unsubscribe(EventType::PLAYER_DIED, this);
    EventBus::getInstance().unsubscribe(EventType::LEVEL_COMPLETED, this);
    EventBus::getInstance().unsubscribe(EventType::GAME_PAUSED, this);
}

void PlayState::rebindCommands() {
    m_inputHandler.clear(); // Reset handlers

    if (m_level->getMario()) {
        // Only continuous-poll commands (movement) go through InputHandler.
        // Jump and Pause are edge-triggered via processEvents().
        m_inputHandler.bindKey(sf::Keyboard::Key::A, std::make_unique<MoveLeftCommand>(m_level->getMario()));
        m_inputHandler.bindKey(sf::Keyboard::Key::Left, std::make_unique<MoveLeftCommand>(m_level->getMario()));
        
        m_inputHandler.bindKey(sf::Keyboard::Key::D, std::make_unique<MoveRightCommand>(m_level->getMario()));
        m_inputHandler.bindKey(sf::Keyboard::Key::Right, std::make_unique<MoveRightCommand>(m_level->getMario()));
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

void PlayState::onNotify(EventType event) {
    if (event == EventType::PLAYER_DIED) {
        if (m_level->getMario() && m_level->getMario()->getLives() <= 0) {
            m_needsGameOver = true;
        } else {
            // Defer reload to next frame — may be called during Box2D step
            m_needsReload = true;
        }
    } else if (event == EventType::LEVEL_COMPLETED) {
        m_currentLevel++;
        if (m_currentLevel > MAX_LEVELS) {
            m_isFading = true;
        } else {
            m_needsReload = true;
        }
    } else if (event == EventType::GAME_PAUSED) {
        GameManager::getInstance().pushState(std::make_unique<PauseState>());
    }
}

void PlayState::processEvents(const sf::Event& event) {
    // Edge-triggered actions (jump, pause) — fire once per key press
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::W ||
            keyPressed->code == sf::Keyboard::Key::Up ||
            keyPressed->code == sf::Keyboard::Key::Space) {
            if (m_level->getMario()) {
                m_level->getMario()->jump();
            }
        }
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            GameManager::getInstance().pushState(std::make_unique<PauseState>());
        }
    }

    if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()) {
        if (keyReleased->code == sf::Keyboard::Key::A || 
            keyReleased->code == sf::Keyboard::Key::Left ||
            keyReleased->code == sf::Keyboard::Key::D || 
            keyReleased->code == sf::Keyboard::Key::Right) {
            if (m_level->getMario()) {
                m_level->getMario()->stopMoving();
            }
        }
    }
}

void PlayState::update(float dt) {
    // Handle deferred state changes (safe: outside Box2D step)
    if (m_needsGameOver) {
        m_needsGameOver = false;
        GameManager::getInstance().changeState(std::make_unique<GameOverState>());
        return;
    }
    if (m_needsReload) {
        m_needsReload = false;
        m_level = std::make_unique<Level>();
        m_level->loadFromFile(getCurrentLevelPath());
        rebindCommands();
        if (m_level->getMario()) {
            m_hud = std::make_unique<HUD>(*(m_level->getMario()), 1, m_currentLevel);
        }
        return;
    }

    // Process continuous inputs via InputHandler
    m_inputHandler.handleInput();

    // Physics is now updated inside Level::update()

    // Update level entities
    m_level->update(dt);

    if (m_hud) {
        m_hud->update();
    }

    if (m_isFading) {
        m_fadeAlpha += (255.f / m_fadeDuration) * dt;
        if (m_fadeAlpha >= 255.f) {
            m_fadeAlpha = 255.f;
            m_fadeOverlay.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(m_fadeAlpha)));
            GameManager::getInstance().changeState(std::make_unique<WinState>());
            return;
        }
        m_fadeOverlay.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(m_fadeAlpha)));
    }
}

void PlayState::render(sf::RenderWindow& window) {
    m_level->render(window);

    // Switch to default view for UI overlay
    window.setView(window.getDefaultView());
    if (m_hud) {
        m_hud->draw(window);
    }

    if (m_isFading || m_fadeAlpha > 0.f) {
        m_fadeOverlay.setSize(sf::Vector2f(window.getSize()));
        window.draw(m_fadeOverlay);
    }
}

std::string PlayState::getCurrentLevelPath() const {
    return "levels/level" + std::to_string(m_currentLevel) + ".txt";
}
