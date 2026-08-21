/**
 * @file CheepCheepTests.cpp
 * @author TV4 (Vy) & CS202 Team
 * @brief Unit tests for Cheep Cheep enemy behaviors, physics, animations, and spawning
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <box2d/box2d.h>

#include "entities/CheepCheep.h"
#include "entities/Mario.h"
#include "patterns/EntityFactory.h"
#include "physics/PhysicsEngine.h"
#include "physics/CollisionManager.h"
#include "level/CheepCheepConfig.h"
#include "level/TileMap.h"
#include <fstream>

void testCheepCheepCreation() {
    std::cout << "[RUNNING] testCheepCheepCreation..." << std::endl;

    b2Vec2 gravity(0.f, 25.f);
    b2World world(gravity);

    // 1. Factory create by type
    auto enemy = EntityFactory::createEnemy(EnemyType::CHEEP_CHEEP, {100.f, 200.f}, &world, LevelTheme::OVERWORLD);
    assert(enemy != nullptr);
    assert(enemy->getType() == Entity::EntityType::ENEMY);
    assert(enemy->isEnemy());
    assert(enemy->isCheepCheep());

    CheepCheep* cheep = static_cast<CheepCheep*>(enemy.get());
    assert(cheep->getBehavior() == CheepCheepBehavior::SWIMMING);
    assert(cheep->getColor() == CheepCheepColor::GREEN);
    assert(!cheep->canBeStomped());

    // 2. Factory create from tile code 'c' (Swimming Green Cheep Cheep)
    auto swimCheep = EntityFactory::createFromTileCode('c', {50.f, 150.f}, &world, LevelTheme::UNDERWATER);
    assert(swimCheep != nullptr);
    assert(swimCheep->isCheepCheep());
    assert(swimCheep->getSubtype() == Entity::EntitySubtype::CHEEP_CHEEP);
    assert(!swimCheep->hasCapability(Entity::Capability::STOMPABLE));
    assert(static_cast<CheepCheep*>(swimCheep.get())->getBehavior() == CheepCheepBehavior::SWIMMING);

    // 3. Direct create Jumping Red Cheep Cheep
    CheepCheep jumpCheep({80.f, 300.f}, &world, LevelTheme::OVERWORLD, CheepCheepBehavior::JUMPING, CheepCheepColor::RED, {0.f, -600.f});
    assert(jumpCheep.isCheepCheep());
    assert(jumpCheep.getSubtype() == Entity::EntitySubtype::CHEEP_CHEEP_JUMPING);
    assert(jumpCheep.hasCapability(Entity::Capability::STOMPABLE));
    assert(jumpCheep.getBehavior() == CheepCheepBehavior::JUMPING);
    assert(jumpCheep.getColor() == CheepCheepColor::RED);
    assert(jumpCheep.canBeStomped());

    std::cout << "[PASSED] testCheepCheepCreation" << std::endl;
}

void testSwimmingCheepCheepMovement() {
    std::cout << "[RUNNING] testSwimmingCheepCheepMovement..." << std::endl;

    b2Vec2 gravity(0.f, 25.f);
    b2World world(gravity);

    CheepCheep cheep({100.f, 100.f}, &world, LevelTheme::UNDERWATER, CheepCheepBehavior::SWIMMING, CheepCheepColor::GREEN);
    cheep.setSwimSpeed(60.f);

    assert(cheep.getFacingDirection() == Direction::LEFT);

    // Run update step
    cheep.update(0.1f);
    sf::Vector2f vel = cheep.getVelocity();
    assert(std::abs(vel.x - (-60.f)) < 0.1f);
    assert(std::abs(vel.y) < 0.1f);

    // Wall collision should reverse direction
    cheep.onWallCollision();
    assert(cheep.getFacingDirection() == Direction::RIGHT);
    cheep.update(0.1f);
    vel = cheep.getVelocity();
    assert(std::abs(vel.x - 60.f) < 0.1f);

    // Red Cheep Cheep with wave modulation
    CheepCheep redCheep({100.f, 100.f}, &world, LevelTheme::OVERWORLD, CheepCheepBehavior::SWIMMING, CheepCheepColor::RED);
    redCheep.setWaveParameters(20.f, 4.f);
    redCheep.update(0.05f);
    sf::Vector2f redVel = redCheep.getVelocity();
    assert(redVel.x < 0.f); // Swimming left
    assert(redVel.y != 0.f); // Has vertical wave component

    std::cout << "[PASSED] testSwimmingCheepCheepMovement" << std::endl;
}

void testJumpingCheepCheepPhysicsAndStomp() {
    std::cout << "[RUNNING] testJumpingCheepCheepPhysicsAndStomp..." << std::endl;

    b2Vec2 gravity(0.f, 25.f);
    b2World world(gravity);

    // Jumping Cheep Cheep launched upward
    CheepCheep jumpCheep({200.f, 400.f}, &world, LevelTheme::OVERWORLD, CheepCheepBehavior::JUMPING, CheepCheepColor::RED, {50.f, -600.f});
    assert(jumpCheep.canBeStomped());
    assert(jumpCheep.getFacingDirection() == Direction::RIGHT);

    // Stomp test
    Mario mario({200.f, 370.f}, {28.f, 30.f});
    mario.initPhysics(&world, b2_dynamicBody, {28.f, 30.f});

    const int initialScore = mario.getScore();
    const bool stomped = CollisionManager::defeatEnemy(jumpCheep, DefeatCause::STOMP, &mario);
    assert(stomped);
    assert(jumpCheep.isDying());
    assert(mario.getScore() > initialScore);

    std::cout << "[PASSED] testJumpingCheepCheepPhysicsAndStomp" << std::endl;
}

void testCheepCheepFireballDefeat() {
    std::cout << "[RUNNING] testCheepCheepFireballDefeat..." << std::endl;

    b2Vec2 gravity(0.f, 25.f);
    b2World world(gravity);

    CheepCheep cheep({150.f, 150.f}, &world, LevelTheme::OVERWORLD, CheepCheepBehavior::SWIMMING, CheepCheepColor::GREEN);
    Mario mario({100.f, 150.f}, {28.f, 30.f});

    const int initialScore = mario.getScore();
    const bool defeated = CollisionManager::defeatEnemy(cheep, DefeatCause::FIREBALL, &mario);
    assert(defeated);
    assert(cheep.isDying());
    assert(mario.getScore() == initialScore + 200);

    std::cout << "[PASSED] testCheepCheepFireballDefeat" << std::endl;
}

void testCheepCheepConfig() {
    std::cout << "[RUNNING] testCheepCheepConfig..." << std::endl;

    // Verify config registry queries do not crash on arbitrary level paths
    std::string level2Path = "levels/level2.txt";
    const auto& spawns = CheepCheepConfig::spawnsFor(level2Path);
    (void)spawns;

    std::string level1Path = "levels/level1.txt";
    const auto& generators = CheepCheepConfig::generatorsFor(level1Path);
    (void)generators;

    std::string nonExistentPath = "non_existent_level.txt";
    const auto& emptySpawns = CheepCheepConfig::spawnsFor(nonExistentPath);
    assert(emptySpawns.empty());

    std::cout << "[PASSED] testCheepCheepConfig" << std::endl;
}

void testCheepCheepRoutes() {
    std::cout << "[RUNNING] testCheepCheepRoutes..." << std::endl;

    b2Vec2 gravity(0.f, 25.f);
    b2World world(gravity);

    // 1. Vertical Route (Fly up and down between row 100 and row 200 at col 50 with speed 40 px/s)
    CheepCheep vertCheep({50.f, 100.f}, &world, LevelTheme::OVERWORLD, CheepCheepBehavior::SWIMMING, CheepCheepColor::RED);
    vertCheep.setRoute({50.f, 100.f}, {50.f, 200.f}, 40.0f);
    assert(vertCheep.hasRoute());
    assert(vertCheep.getRouteAxis() == CheepCheep::RouteAxis::VERTICAL);

    vertCheep.update(0.1f);
    sf::Vector2f vVel = vertCheep.getVelocity();
    assert(std::abs(vVel.x) < 0.1f);
    assert(std::abs(vVel.y - 40.f) < 0.1f);

    // Reaching bottom boundary: should flip direction upward
    vertCheep.setPosition({50.f, 205.f});
    vertCheep.update(0.1f);
    vVel = vertCheep.getVelocity();
    assert(std::abs(vVel.y - (-40.f)) < 0.1f);

    // 2. Horizontal Route (Patrol between col 100 and col 300 at row 150 with speed 50 px/s)
    CheepCheep horizCheep({100.f, 150.f}, &world, LevelTheme::OVERWORLD, CheepCheepBehavior::SWIMMING, CheepCheepColor::GREEN);
    horizCheep.setRoute({100.f, 150.f}, {300.f, 150.f}, 50.0f);
    assert(horizCheep.hasRoute());
    assert(horizCheep.getRouteAxis() == CheepCheep::RouteAxis::HORIZONTAL);

    horizCheep.update(0.1f);
    sf::Vector2f hVel = horizCheep.getVelocity();
    assert(std::abs(hVel.x - 50.f) < 0.1f);
    assert(horizCheep.getFacingDirection() == Direction::RIGHT);

    // Reaching right boundary: should flip direction to left
    horizCheep.setPosition({305.f, 150.f});
    horizCheep.update(0.1f);
    hVel = horizCheep.getVelocity();
    assert(std::abs(hVel.x - (-50.f)) < 0.1f);
    assert(horizCheep.getFacingDirection() == Direction::LEFT);

    std::cout << "[PASSED] testCheepCheepRoutes" << std::endl;
}

void testTileMapCheepCheepRouteParsing() {
    std::cout << "[RUNNING] testTileMapCheepCheepRouteParsing..." << std::endl;

    TileMap map1;
    assert(map1.loadFromFile("levels/level1.txt"));

    TileMap map2;
    assert(map2.loadFromFile("levels/level2.txt"));
    const auto& routes2 = map2.getCheepCheepRoutes();
    assert(routes2.size() == 2u);
    assert(routes2[0].start == sf::Vector2i(204, 12));
    assert(routes2[0].end == sf::Vector2i(204, 4));
    assert(routes2[0].vertical);
    assert(routes2[1].start == sf::Vector2i(189, 13));
    assert(routes2[1].end == sf::Vector2i(189, 7));
    assert(routes2[1].vertical);

    std::cout << "[PASSED] testTileMapCheepCheepRouteParsing" << std::endl;
}

int main() {
    std::cout << "=== Running Cheep Cheep Tests ===" << std::endl;
    testCheepCheepCreation();
    testSwimmingCheepCheepMovement();
    testJumpingCheepCheepPhysicsAndStomp();
    testCheepCheepFireballDefeat();
    testCheepCheepRoutes();
    testCheepCheepConfig();
    testTileMapCheepCheepRouteParsing();
    std::cout << "All Cheep Cheep tests PASSED successfully!" << std::endl;
    return 0;
}
