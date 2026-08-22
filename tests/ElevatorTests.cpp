/**
 * @file ElevatorTests.cpp
 * @author TV1 (Duong)
 * @brief Unit tests for the kinematic moving platform / lift entity
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>

#include <box2d/box2d.h>

#include "entities/Elevator.h"
#include "entities/Mario.h"
#include "level/ElevatorConfig.h"
#include "level/TileMap.h"
#include "physics/PhysicsEngine.h"

namespace {
constexpr float TIME_STEP = 1.0f / 60.0f;
constexpr int VELOCITY_ITERATIONS = 8;
constexpr int POSITION_ITERATIONS = 3;

void step(b2World& world, Elevator& elevator, int frames) {
    for (int i = 0; i < frames; ++i) {
        world.Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
        elevator.update(TIME_STEP);
    }
}

float centerY(const Elevator& elevator) {
    return PhysicsEngine::metersToPixels(elevator.getBody()->GetPosition()).y;
}
} // namespace

void testElevatorCreation() {
    std::cout << "[RUNNING] testElevatorCreation..." << std::endl;

    b2World world(b2Vec2(0.f, 25.f));
    Elevator elevator({100.f, 300.f}, {100.f, 100.f}, 100.f,
                      Elevator::Axis::VERTICAL, 0.5f, LevelTheme::UNDERGROUND);

    assert(elevator.getType() == Entity::EntityType::ELEVATOR);
    assert(elevator.isElevator());
    assert(elevator.getAxis() == Elevator::Axis::VERTICAL);
    assert(std::abs(elevator.getSpeed() - 100.f) < 0.001f);
    assert(elevator.getDirection() == 1);

    elevator.initPhysics(&world, b2_dynamicBody, elevator.getSize());
    assert(elevator.getBody()->GetType() == b2_kinematicBody);

    // Kinematic bodies ignore gravity: no motion without an update() velocity.
    world.Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
    elevator.update(TIME_STEP);
    assert(std::abs(centerY(elevator) - 308.f) < 0.5f);

    std::cout << "[PASSED] testElevatorCreation" << std::endl;
}

void testElevatorVerticalCycle() {
    std::cout << "[RUNNING] testElevatorVerticalCycle..." << std::endl;

    b2World world(b2Vec2(0.f, 25.f));
    const sf::Vector2f start(100.f, 300.f);
    const sf::Vector2f end(100.f, 100.f);
    Elevator elevator(start, end, 100.f, Elevator::Axis::VERTICAL, 0.5f);
    elevator.initPhysics(&world, b2_dynamicBody, elevator.getSize());
    const float startCenter = start.y + elevator.getSize().y / 2.0f;

    // 1. Moves upward (toward the end marker) at full speed.
    step(world, elevator, 60);
    assert(centerY(elevator) < startCenter - 90.f);
    assert(elevator.getBody()->GetLinearVelocity().y < 0.f);
    assert(!elevator.isPaused());

    // 2. Arrives at the end marker and pauses (200 px / 100 px/s = 2.0 s).
    step(world, elevator, 75);
    const float endCenter = end.y + elevator.getSize().y / 2.0f;
    assert(std::abs(centerY(elevator) - endCenter) < 3.f);
    assert(elevator.isPaused());
    assert(std::abs(elevator.getBody()->GetLinearVelocity().y) < 0.001f);

    // 3. Pause expires (0.5 s) and it reverses back toward the start.
    step(world, elevator, 30);
    assert(elevator.getBody()->GetLinearVelocity().y > 0.f);
    assert(elevator.getDirection() == -1);

    // 4. Full round trip: returns at 4.5 s, then pauses 0.5 s at the start.
    //    60 + 75 + 30 + 110 = 275 frames = 4.58 s (inside the start pause).
    step(world, elevator, 110);
    assert(std::abs(centerY(elevator) - startCenter) < 3.f);
    assert(elevator.isPaused());
    assert(std::abs(elevator.getBody()->GetLinearVelocity().y) < 0.001f);

    // 5. Start pause expires at 5.0 s; the cycle restarts upward.
    step(world, elevator, 40);
    assert(elevator.getBody()->GetLinearVelocity().y < 0.f);
    assert(elevator.getDirection() == 1);

    std::cout << "[PASSED] testElevatorVerticalCycle" << std::endl;
}

void testElevatorHorizontalMotion() {
    std::cout << "[RUNNING] testElevatorHorizontalMotion..." << std::endl;

    b2World world(b2Vec2(0.f, 25.f));
    const sf::Vector2f start(100.f, 200.f);
    const sf::Vector2f end(400.f, 200.f);
    Elevator elevator(start, end, 150.f, Elevator::Axis::HORIZONTAL, 0.0f);
    elevator.initPhysics(&world, b2_dynamicBody, elevator.getSize());

    // No pause configured: it must keep moving, alternating direction.
    step(world, elevator, 120);
    float x = PhysicsEngine::metersToPixels(elevator.getBody()->GetPosition()).x;
    const float endCenter = end.x + elevator.getSize().x / 2.0f;
    assert(std::abs(x - endCenter) < 3.f);

    step(world, elevator, 30);
    assert(elevator.getBody()->GetLinearVelocity().x < 0.f);

    // It must not drift vertically on a horizontal route.
    float y = PhysicsEngine::metersToPixels(elevator.getBody()->GetPosition()).y;
    const float startCenter = start.y + elevator.getSize().y / 2.0f;
    assert(std::abs(y - startCenter) < 1.f);

    std::cout << "[PASSED] testElevatorHorizontalMotion" << std::endl;
}

void testElevatorCarriesMario() {
    std::cout << "[RUNNING] testElevatorCarriesMario..." << std::endl;

    b2World world(b2Vec2(0.f, 25.f));
    const sf::Vector2f start(100.f, 300.f);
    const sf::Vector2f end(100.f, 100.f);
    Elevator elevator(start, end, 80.f, Elevator::Axis::VERTICAL, 0.5f);
    elevator.initPhysics(&world, b2_dynamicBody, elevator.getSize());

    // Mario stands on top of the platform.
    Mario mario({start.x + 16.f, start.y - 30.f}, {32.f, 30.f});
    mario.initPhysics(&world, b2_dynamicBody, {32.f, 30.f});
    b2Body* marioBody = mario.getBody();
    assert(marioBody != nullptr);

    // Mario's getPosition() caches m_position via syncPhysics(), which only
    // Mario::update() refreshes; read the live body position instead.
    const auto marioBottom = [&marioBody]() {
        return PhysicsEngine::metersToPixels(marioBody->GetPosition()).y + 15.f;
    };

    // Let Mario settle onto the platform, then ride it to the top.
    step(world, elevator, 30);
    const float settledBottom = marioBottom();
    step(world, elevator, 150);

    const float platformTop = centerY(elevator) - elevator.getSize().y / 2.0f;
    assert(marioBottom() < settledBottom - 120.f);
    assert(std::abs(marioBottom() - platformTop) < 6.f);

    std::cout << "[PASSED] testElevatorCarriesMario" << std::endl;
}

void testElevatorConfig() {
    std::cout << "[RUNNING] testElevatorConfig..." << std::endl;

    const std::vector<std::pair<std::string, std::size_t>> expectations = {
        {"level1.txt", 0u},
        {"level2.txt", 0u},
        {"level3.txt", 0u},
        {"level4.txt", 0u},
    };

    for (const auto& [levelFile, expectedCount] : expectations) {
        const auto& routes = ElevatorConfig::routesFor(levelFile);

        if (routes.size() != expectedCount) {
            std::cerr << "Expected " << expectedCount << " routes for " << levelFile
                      << " but got " << routes.size() << std::endl;
            std::abort();
        }

        TileMap map;
        if (!map.loadFromFile("levels/" + levelFile)) {
            std::cerr << "Failed to load levels/" << levelFile << std::endl;
            std::abort();
        }

        const int width = static_cast<int>(map.getWidth());
        const int height = static_cast<int>(map.getHeight());

        for (const auto& route : routes) {
            const auto& s = route.start;
            const auto& e = route.end;

            // Bounds.
            assert(s.x >= 0 && s.x < width && s.y >= 0 && s.y < height);
            assert(e.x >= 0 && e.x < width && e.y >= 0 && e.y < height);

            // Must span at least one tile.
            assert(s.x != e.x || s.y != e.y);

            // Must be vertical (same column) or horizontal (same row).
            const bool vertical = (s.x == e.x);
            assert(vertical != (s.y == e.y));

            // Endpoints must be on non-solid, non-climbable tiles so the
            // platform has somewhere to sit and riders can board.
            if (map.isSolid(s.x, s.y) || map.isSolid(e.x, e.y)) {
                std::cerr << "SOLID at (" << s.x << "," << s.y << ")="
                          << (map.isSolid(s.x, s.y) ? "T" : "F")
                          << " (" << e.x << "," << e.y << ")="
                          << (map.isSolid(e.x, e.y) ? "T" : "F")
                          << " in " << levelFile << std::endl;
                std::abort();
            }
            assert(!map.isSolid(s.x, s.y));
            assert(!map.isSolid(e.x, e.y));
            assert(!map.isClimbable(s.x, s.y));
            assert(!map.isClimbable(e.x, e.y));

            // Sanity on the numeric defaults.
            assert(route.speedPixelsPerSecond > 0.f);
            assert(route.pauseSeconds >= 0.f);
        }
    }

    // Unknown / unregistered levels must yield an empty route list.
    assert(ElevatorConfig::routesFor("does_not_exist.txt").empty());

    // Level 2 keeps two active elevators in the map itself via '^'/'~'.
    // The external registry remains available but has no default routes.
    TileMap markerMap;
    assert(markerMap.loadFromFile("levels/level2.txt"));
    const auto& markerRoutes = markerMap.getElevatorRoutes();
    assert(markerRoutes.size() == 2u);
    assert(markerRoutes[0].start == sf::Vector2i(206, 12));
    assert(markerRoutes[0].end == sf::Vector2i(206, 4));
    assert(markerRoutes[0].vertical);
    assert(markerRoutes[1].start == sf::Vector2i(191, 13));
    assert(markerRoutes[1].end == sf::Vector2i(191, 7));
    assert(markerRoutes[1].vertical);

    std::cout << "[PASSED] testElevatorConfig" << std::endl;
}

int main() {
    std::cout << "=== Running Elevator Tests ===" << std::endl;
    testElevatorCreation();
    testElevatorVerticalCycle();
    testElevatorHorizontalMotion();
    testElevatorCarriesMario();
    testElevatorConfig();

    // --- probe clear spans for config route selection ---
    for (const auto& lvl : {"level1.txt", "level2.txt", "level3.txt", "level4.txt"}) {
        TileMap map;
        if (!map.loadFromFile(std::string("levels/") + lvl)) continue;
        const int w = (int)map.getWidth(), h = (int)map.getHeight();
        std::cout << "\n== " << lvl << " (w=" << w << " h=" << h << ") HORIZONTAL clear runs ==" << std::endl;
        for (int r = 0; r < h; ++r) {
            int c = 0;
            while (c < w) {
                if (map.isSolid(c, r)) { ++c; continue; }
                if (map.isClimbable(c, r)) { ++c; continue; }
                int start = c;
                while (c < w && !map.isSolid(c, r) && !map.isClimbable(c, r)) ++c;
                int len = c - start;
                if (len >= 6) std::cout << "  row " << r << " cols " << start << ".." << (c - 1) << " (len " << len << ")" << std::endl;
            }
        }
        std::cout << "== " << lvl << " VERTICAL clear runs ==" << std::endl;
        for (int c = 0; c < w; ++c) {
            int r = 0;
            while (r < h) {
                if (map.isSolid(c, r)) { ++r; continue; }
                if (map.isClimbable(c, r)) { ++r; continue; }
                int start = r;
                while (r < h && !map.isSolid(c, r) && !map.isClimbable(c, r)) ++r;
                int len = r - start;
                if (len >= 5) std::cout << "  col " << c << " rows " << start << ".." << (r - 1) << " (len " << len << ")" << std::endl;
            }
        }
    }
    return 0;
}
