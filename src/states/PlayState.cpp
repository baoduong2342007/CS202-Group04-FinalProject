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
#include "core/GameManager.h"
#include "core/SoundManager.h"
#include <cstdint>

namespace {
    const sf::Color FADE_START_COLOR(0, 0, 0, 0);
}

PlayState::PlayState() {
    m_level = std::make_unique<Level>();
    // Load Level 1
    m_level->loadFromFile("levels/level1.txt");

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
        m_inputHandler.bindKey(sf::Keyboard::Key::W, std::make_unique<JumpCommand>(m_level->getMario()));
        m_inputHandler.bindKey(sf::Keyboard::Key::Up, std::make_unique<JumpCommand>(m_level->getMario()));
        m_inputHandler.bindKey(sf::Keyboard::Key::Space, std::make_unique<JumpCommand>(m_level->getMario()));
        
        m_inputHandler.bindKey(sf::Keyboard::Key::A, std::make_unique<MoveLeftCommand>(m_level->getMario()));
        m_inputHandler.bindKey(sf::Keyboard::Key::Left, std::make_unique<MoveLeftCommand>(m_level->getMario()));
        
        m_inputHandler.bindKey(sf::Keyboard::Key::D, std::make_unique<MoveRightCommand>(m_level->getMario()));
        m_inputHandler.bindKey(sf::Keyboard::Key::Right, std::make_unique<MoveRightCommand>(m_level->getMario()));
    }

    // Pause command
    m_inputHandler.bindKey(sf::Keyboard::Key::Escape, std::make_unique<PauseCommand>(&GameManager::getInstance()));
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
            GameManager::getInstance().changeState(std::make_unique<GameOverState>());
        } else {
            // Reload level
            m_level = std::make_unique<Level>();
            m_level->loadFromFile("levels/level1.txt");
            rebindCommands();
            if (m_level->getMario()) {
                m_hud = std::make_unique<HUD>(*(m_level->getMario()));
            }
        }
    } else if (event == EventType::LEVEL_COMPLETED) {
        m_isFading = true;
    } else if (event == EventType::GAME_PAUSED) {
        GameManager::getInstance().pushState(std::make_unique<PauseState>());
    }
}

void PlayState::processEvents(const sf::Event& event) {
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
