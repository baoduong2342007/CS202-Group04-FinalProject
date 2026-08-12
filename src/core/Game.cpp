/**
 * @file Game.cpp
 * @author TV2
 * @brief Game loop, window management and event handling
 * @note Sprint 4 fix — delegates logic to GameManager (State Pattern)
 */
#include "core/Game.h"
#include "core/DisplayConfig.h"
#include "core/GameManager.h"
#include "core/SoundManager.h"
#include "states/MenuState.h"
#include <iostream>
#include <memory>

namespace {
    /**
     * @brief The maximum delta time in seconds. Clamps the frame time to prevent huge physics jumps.
     */
    constexpr float MAX_DELTA_TIME = 0.1f;

    std::optional<sf::Event> mapMouseEventToLogical(
        const sf::Event& event, const sf::Vector2u& windowSize) {
      if (const auto* pressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        const auto position = DisplayConfig::mapPhysicalToLogical(
            pressed->position, windowSize);
        if (!position) return std::nullopt;
        return sf::Event(sf::Event::MouseButtonPressed{
            pressed->button, *position});
      }
      if (const auto* released = event.getIf<sf::Event::MouseButtonReleased>()) {
        const auto position = DisplayConfig::mapPhysicalToLogical(
            released->position, windowSize);
        if (!position) return std::nullopt;
        return sf::Event(sf::Event::MouseButtonReleased{
            released->button, *position});
      }
      if (const auto* moved = event.getIf<sf::Event::MouseMoved>()) {
        const auto position = DisplayConfig::mapPhysicalToLogical(
            moved->position, windowSize);
        if (!position) return std::nullopt;
        return sf::Event(sf::Event::MouseMoved{*position});
      }
      return event;
    }
} // namespace

Game::Game()
    : m_window(sf::VideoMode(
                   {DisplayConfig::WINDOW_WIDTH, DisplayConfig::WINDOW_HEIGHT}),
               "Super Mario - CS202 Group 04",
               sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize) {
  m_window.setFramerateLimit(DisplayConfig::FRAMERATE_LIMIT);
  m_window.setMinimumSize(sf::Vector2u{
      DisplayConfig::LOGICAL_WIDTH, DisplayConfig::LOGICAL_HEIGHT});

  if (!m_renderTexture.resize(
          {DisplayConfig::LOGICAL_WIDTH, DisplayConfig::LOGICAL_HEIGHT})) {
    std::cerr << "Failed to create render texture!" << std::endl;
  }
  m_renderTexture.setSmooth(false);

  // Initialize SoundManager singleton — constructor subscribes to EventBus
  // and preloads all sound effects/music. Without this call the audio
  // system stays dead (no jump/coin sounds at all).
  SoundManager& soundManager = SoundManager::getInstance();

  // Apply the single SaveManager-owned audio settings at the composition
  // root. SoundManager remains the runtime mixer; SaveManager remains the
  // persistence owner, so no second SaveManager is created in a state.
  GameManager& gameManager = GameManager::getInstance();
  const SaveData& savedAudio = gameManager.getSaveManager().getData();
  soundManager.setSoundVolume(savedAudio.soundVolume);
  soundManager.setMusicVolume(savedAudio.musicVolume);

  // Transition to MenuState as the initial state
  gameManager.changeState(std::make_unique<MenuState>());
}
void Game::run() {
#ifdef DEBUG
  std::cout << "Starting SFML window. Press ESC or close the window to exit."
            << std::endl;
#endif

  sf::Clock clock;
  while (m_window.isOpen()) {
    sf::Time deltaTime = clock.restart();

    // Convert sf::Time to float (seconds) for the architecture
    float dt = deltaTime.asSeconds();
    if (dt > MAX_DELTA_TIME)
      dt = MAX_DELTA_TIME; // clamp delta time

    processEvents();
    if (!m_window.isOpen())
      break;
    update(dt);
    render();
  }

#ifdef DEBUG
  std::cout << "Window closed successfully." << std::endl;
#endif
}

void Game::processEvents() {
  m_inputState.beginFrame();

  while (const std::optional event = m_window.pollEvent()) {
    m_inputState.handleEvent(*event);

    if (event->is<sf::Event::Closed>()) {
      m_window.close();
      return;
    }
    
    // States consume logical mouse coordinates. A mouse event in a black bar
    // is rejected, while keyboard/window events pass through unchanged.
    if (const auto mapped = mapMouseEventToLogical(*event, m_window.getSize())) {
      GameManager::getInstance().processEvents(*mapped);
    }
  }
}

void Game::update(float dt) {
  GameManager::getInstance().processInput(m_inputState);
  GameManager::getInstance().update(dt);
  
  if (GameManager::getInstance().stackDepth() == 0) {
      m_window.close();
  }
}
void Game::render() {
  m_renderTexture.clear(DisplayConfig::BACKGROUND_COLOR); // Sky blue background

  // Delegate rendering to GameManager
  GameManager::getInstance().render(m_renderTexture);

  m_renderTexture.display();

  m_window.clear(sf::Color::Black);

  sf::Sprite renderSprite(m_renderTexture.getTexture());
  const DisplayConfig::IntegerViewport viewport =
      DisplayConfig::calculateIntegerViewport(m_window.getSize());
  const float scale = static_cast<float>(viewport.scale);
  renderSprite.setScale({scale, scale});
  renderSprite.setPosition({static_cast<float>(viewport.position.x),
                            static_cast<float>(viewport.position.y)});

  // Explicitly set the view to match the physical window size 
  // to prevent SFML from applying a second layer of scaling
  sf::Vector2f windowSize(static_cast<float>(m_window.getSize().x), static_cast<float>(m_window.getSize().y));
  sf::View view(windowSize / 2.f, windowSize);
  m_window.setView(view);

  m_window.draw(renderSprite);
  m_window.display();
}
