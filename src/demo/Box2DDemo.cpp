#include "entities/Mario.h"
#include "physics/PhysicsEngine.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>


int runBox2DDemo() {
  // Define standard HD 16:9 window size
  const unsigned int windowWidth = 1280;
  const unsigned int windowHeight = 720;

  // Create a game window with fixed size (1280x720) and disable resizing
  sf::RenderWindow window(sf::VideoMode({windowWidth, windowHeight}),
                          "Super Mario - Box2D Physics Test",
                          sf::Style::Titlebar | sf::Style::Close);
  window.setFramerateLimit(60);

  // Initialize Box2D Physics Engine with downward gravity (approx. 15 m/s^2)
  // Positive Y is down in SFML coordinates, so gravity is positive Y.
  PhysicsEngine::getInstance().init(sf::Vector2f(0.0f, 900.0f));

  // Get the world reference
  b2World *world = PhysicsEngine::getInstance().getWorld();

  // 1. Create a static ground body at the bottom of the screen
  b2BodyDef floorBodyDef;
  floorBodyDef.type = b2_staticBody;
  // Set floor center position (X: middle of screen, Y: near the bottom)
  floorBodyDef.position =
      PhysicsEngine::pixelsToMeters(sf::Vector2f(640.f, 650.f));
  b2Body *floorBody = world->CreateBody(&floorBodyDef);

  b2PolygonShape floorBox;
  // Box2D SetAsBox takes half-width and half-height
  floorBox.SetAsBox(PhysicsEngine::pixelsToMeters(640.f),
                    PhysicsEngine::pixelsToMeters(25.f));
  floorBody->CreateFixture(&floorBox, 0.0f);

  // Visual representation for the floor
  sf::RectangleShape floorShape(sf::Vector2f(1280.f, 50.f));
  floorShape.setFillColor(sf::Color(127, 140, 141)); // Pleasant grey color
  floorShape.setPosition({0.f, 625.f});

  // 2. Create dynamic Mario character
  Mario mario(sf::Vector2f(620.f, 100.f), sf::Vector2f(40.f, 40.f));
  mario.initPhysics(b2_dynamicBody, sf::Vector2f(40.f, 40.f));

  // Visual representation for Mario (Red square)
  sf::RectangleShape marioShape(sf::Vector2f(40.f, 40.f));
  marioShape.setFillColor(sf::Color(231, 76, 60)); // Mario red

  std::cout << "Starting Box2D Physics Demo." << std::endl;
  std::cout << "Controls: Left/Right Arrow or A/D to move, Space/W or Up Arrow "
               "to jump."
            << std::endl;
  std::cout << "Press ESC or close the window to exit." << std::endl;

  sf::Clock clock;

  // Game loop following SFML 3.0.0 standards
  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      // Close the window when clicking the X button
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
      // Close the window when pressing the Escape key
      else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
          window.close();
        }
      }
    }

    // Measure elapsed time
    float dt = clock.restart().asSeconds();
    if (dt > 0.1f)
      dt = 0.1f; // Cap delta time during lag spikes

    // Handle controls and update physics
    mario.handleInput();
    PhysicsEngine::getInstance().update(dt);
    mario.update(dt);

    // Update visual position of Mario shape from physical position
    marioShape.setPosition(mario.getPosition());

    // Clear the screen with a pleasant dark color
    window.clear(sf::Color(44, 62, 80));

    // Draw static floor and Mario
    window.draw(floorShape);
    window.draw(marioShape);

    // Display the frame
    window.display();
  }

  std::cout << "Demo closed successfully." << std::endl;
  return 0;
}
