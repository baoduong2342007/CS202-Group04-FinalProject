/**
 * @file TV3Demo.cpp
 * @author TV3 (Bảo)
 * @brief Interactive test demo for TV3 features: Mario physics, states, lives, and FireBall bouncing
 */

#include "demo/TV3Demo.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <optional>
#include "entities/Mario.h"
#include "entities/FireBall.h"
#include "physics/PhysicsEngine.h"

namespace {
constexpr unsigned int WINDOW_WIDTH = 1280;
constexpr unsigned int WINDOW_HEIGHT = 720;
constexpr float GRAVITY_Y = 900.0f;

// Floor and wall dimensions
const sf::Vector2f FLOOR_SIZE(1280.f, 50.f);
const sf::Vector2f FLOOR_POS(0.f, 620.f);
const sf::Vector2f WALL_SIZE(50.f, 300.f);
const sf::Vector2f WALL_POS(1100.f, 320.f);
} // namespace

int runTV3Demo() {
  sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}),
                          "Super Mario - TV3 Physics & FireBall Test",
                          sf::Style::Titlebar | sf::Style::Close);
  window.setFramerateLimit(60);

  // Initialize Box2D Physics Engine
  PhysicsEngine::getInstance().init(sf::Vector2f(0.0f, GRAVITY_Y));
  b2World *world = PhysicsEngine::getInstance().getWorld();

  if (!world) {
    std::cerr << "Error: Physics engine world failed to initialize!" << std::endl;
    return -1;
  }

  // 1. Create static floor body in Box2D
  b2BodyDef floorBodyDef;
  floorBodyDef.type = b2_staticBody;
  floorBodyDef.position = PhysicsEngine::pixelsToMeters(sf::Vector2f(640.f, 645.f));
  b2Body *floorBody = world->CreateBody(&floorBodyDef);

  b2PolygonShape floorBox;
  floorBox.SetAsBox(PhysicsEngine::pixelsToMeters(640.f), PhysicsEngine::pixelsToMeters(25.f));
  b2FixtureDef floorFixtureDef;
  floorFixtureDef.shape = &floorBox;
  floorFixtureDef.friction = 0.5f;
  floorBody->CreateFixture(&floorFixtureDef);

  // Visual representation for Floor
  sf::RectangleShape floorShape(FLOOR_SIZE);
  floorShape.setFillColor(sf::Color(46, 204, 113)); // Grass green
  floorShape.setPosition(FLOOR_POS);

  // 2. Create static wall body in Box2D
  b2BodyDef wallBodyDef;
  wallBodyDef.type = b2_staticBody;
  wallBodyDef.position = PhysicsEngine::pixelsToMeters(sf::Vector2f(1125.f, 470.f));
  b2Body *wallBody = world->CreateBody(&wallBodyDef);

  b2PolygonShape wallBox;
  wallBox.SetAsBox(PhysicsEngine::pixelsToMeters(25.f), PhysicsEngine::pixelsToMeters(150.f));
  b2FixtureDef wallFixtureDef;
  wallFixtureDef.shape = &wallBox;
  wallFixtureDef.friction = 0.5f;
  wallBody->CreateFixture(&wallFixtureDef);

  // Visual representation for Wall
  sf::RectangleShape wallShape(WALL_SIZE);
  wallShape.setFillColor(sf::Color(149, 165, 166)); // Grey block
  wallShape.setPosition(WALL_POS);

  // 3. Create Mario entity
  sf::Vector2f startPos(100.f, 400.f);
  sf::Vector2f initialSize(32.f, 32.f);
  Mario mario(startPos, initialSize);
  mario.initPhysics(b2_dynamicBody, initialSize);

  // Visual Mario shape
  sf::RectangleShape marioShape(initialSize);

  // FireBall container
  std::vector<std::unique_ptr<FireBall>> fireballs;

  std::cout << "\n======================================================\n";
  std::cout << "         SUPER MARIO - TV3 INTEGRATION DEMO          \n";
  std::cout << "======================================================\n";
  std::cout << " Controls:\n";
  std::cout << "  - A / D or Left / Right : Move Left / Right\n";
  std::cout << "  - W / Space / Up       : Jump\n";
  std::cout << "  - 1 / 2 / 3           : Switch State (1: SMALL, 2: SUPER, 3: FIRE)\n";
  std::cout << "  - F                   : Shoot FireBall (in FIRE state)\n";
  std::cout << "  - K                   : Power Down / Take Damage\n";
  std::cout << "  - ESC                 : Exit Demo\n";
  std::cout << "======================================================\n\n";

  sf::Clock clock;

  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      } else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
          window.close();
        } else if (keyPressed->code == sf::Keyboard::Key::Num1) {
          mario.setMarioState(MarioState::SMALL);
          std::cout << "[TV3 Demo] Switched Mario state to SMALL" << std::endl;
        } else if (keyPressed->code == sf::Keyboard::Key::Num2) {
          mario.powerUp(MarioState::SUPER);
          std::cout << "[TV3 Demo] Switched Mario state to SUPER" << std::endl;
        } else if (keyPressed->code == sf::Keyboard::Key::Num3) {
          mario.powerUp(MarioState::FIRE);
          std::cout << "[TV3 Demo] Switched Mario state to FIRE" << std::endl;
        } else if (keyPressed->code == sf::Keyboard::Key::F) {
          if (mario.canShootFireBall()) {
            sf::Vector2f fbPos = mario.getPosition() + sf::Vector2f(mario.getFacingDirection() == Direction::RIGHT ? 35.f : -15.f, 10.f);
            auto fb = std::make_unique<FireBall>(fbPos, mario.getFacingDirection());
            fireballs.push_back(std::move(fb));
            std::cout << "[TV3 Demo] FireBall shot! Active fireballs: " << fireballs.size() << std::endl;
          } else {
            std::cout << "[TV3 Demo] Cannot shoot FireBall! Mario must be in FIRE state (Press 3)." << std::endl;
          }
        } else if (keyPressed->code == sf::Keyboard::Key::K) {
          mario.powerDown();
          std::cout << "[TV3 Demo] Mario powerDown() triggered. Remaining lives: " << mario.getLives() << std::endl;
        }
      }
    }

    float dt = clock.restart().asSeconds();
    if (dt > 0.1f) dt = 0.1f;

    // Handle Mario input and physics update
    mario.handleInput();
    PhysicsEngine::getInstance().update(dt);
    mario.update(dt);

    // Update FireBalls
    for (auto it = fireballs.begin(); it != fireballs.end();) {
      (*it)->update(dt);
      if ((*it)->isPendingDestroy() || !(*it)->isActive()) {
        it = fireballs.erase(it);
      } else {
        ++it;
      }
    }

    // Update Mario visual shape based on state and physics position
    marioShape.setSize(mario.getBoundingBox().size);
    marioShape.setPosition(mario.getPosition());

    if (mario.getMarioState() == MarioState::SMALL) {
      marioShape.setFillColor(sf::Color(231, 76, 60)); // Small Red
    } else if (mario.getMarioState() == MarioState::SUPER) {
      marioShape.setFillColor(sf::Color(155, 89, 182)); // Super Purple
    } else if (mario.getMarioState() == MarioState::FIRE) {
      marioShape.setFillColor(sf::Color(230, 126, 34)); // Fire Orange
    }

    // Rendering
    window.clear(sf::Color(44, 62, 80)); // Dark background

    window.draw(floorShape);
    window.draw(wallShape);
    if (mario.isActive()) {
      window.draw(marioShape);
    }

    // Draw active FireBalls
    for (const auto &fb : fireballs) {
      if (fb->isActive()) {
        sf::CircleShape fbShape(6.f);
        fbShape.setFillColor(sf::Color(241, 196, 15)); // Yellow fireball
        fbShape.setPosition(fb->getPosition());
        window.draw(fbShape);
      }
    }

    window.display();
  }

  std::cout << "[TV3 Demo] Demo finished successfully." << std::endl;
  return 0;
}
