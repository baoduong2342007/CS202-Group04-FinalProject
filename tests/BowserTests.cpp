/**
 * @file BowserTests.cpp
 * @brief Unit tests for the Phase 5 boss kit: Bowser's multi-hit fireball
 *        health, fire breath, axe collapse, and bridge demolition flow
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <box2d/box2d.h>

#include "entities/Bowser.h"
#include "entities/BowserAxe.h"
#include "entities/BowserFire.h"
#include "entities/Mario.h"
#include "patterns/EntityFactory.h"
#include "level/TileMap.h"

namespace {

b2World makeWorld() {
    return b2World(b2Vec2(0.f, 25.f));
}

void advance(Entity& entity, float seconds, float dt = 0.05f) {
    for (float t = 0.f; t < seconds; t += dt) {
        entity.update(dt);
    }
}

} // namespace

void testBowserCreation() {
    std::cout << "[RUNNING] testBowserCreation..." << std::endl;

    b2World world = makeWorld();
    auto entity = EntityFactory::createFromTileCode('X', {500.f, 416.f}, &world,
                                                    LevelTheme::CASTLE);
    assert(entity != nullptr);
    assert(entity->isEnemy());
    assert(entity->isBowser());

    Bowser* bowser = static_cast<Bowser*>(entity.get());
    assert(bowser->getState() == Bowser::State::PATROL);
    assert(bowser->getSize() == sf::Vector2f(64.f, 64.f));
    assert(bowser->getFireballHealth() == 5);
    assert(bowser->getHealth() == 5);
    assert(!static_cast<Enemy*>(bowser)->canBeStomped());
    assert(bowser->getDefeatScore(static_cast<int>(DefeatCause::FIREBALL)) == 5000);

    std::cout << "[PASSED] testBowserCreation" << std::endl;
}

void testBowserFireballChips() {
    std::cout << "[RUNNING] testBowserFireballChips..." << std::endl;

    b2World world = makeWorld();
    Bowser bowser({500.f, 416.f}, &world, LevelTheme::CASTLE);

    // Four hits only chip the health pool.
    for (int i = 0; i < 4; ++i) {
        bowser.onFireHit();
        assert(!bowser.isDying());
    }
    assert(bowser.getHealth() == 1);

    // The fifth fireball sends him into the lava.
    bowser.onFireHit();
    assert(bowser.isDying());
    assert(bowser.getState() == Bowser::State::DIE);

    // Extra hits after death change nothing.
    bowser.onFireHit();
    assert(bowser.getState() == Bowser::State::DIE);

    std::cout << "[PASSED] testBowserFireballChips" << std::endl;
}

void testBowserStarIsHarmless() {
    std::cout << "[RUNNING] testBowserStarIsHarmless..." << std::endl;

    b2World world = makeWorld();

    // Canonical SMB1: star contact does NOT harm Bowser — only five
    // fireballs or the axe finish him. Full health is preserved.
    Bowser starred({500.f, 416.f}, &world, LevelTheme::CASTLE);
    starred.onStarHit();
    assert(!starred.isDying());
    assert(starred.getState() == Bowser::State::PATROL);
    assert(starred.getHealth() == 5);

    // Star immunity never weakens the fireball path.
    for (int i = 0; i < 4; ++i) {
        starred.onFireHit();
        assert(!starred.isDying());
    }
    starred.onFireHit();
    assert(starred.isDying());

    std::cout << "[PASSED] testBowserStarIsHarmless" << std::endl;
}

void testBowserAxeCollapseKill() {
    std::cout << "[RUNNING] testBowserAxeCollapseKill..." << std::endl;

    b2World world = makeWorld();

    // The axe's bridge collapse is lethal at full health.
    Bowser axed({600.f, 416.f}, &world, LevelTheme::CASTLE);
    axed.collapseIntoLava();
    assert(axed.isDying());

    // The sinking body drifts downward until cleaned up (the world must be
    // stepped; update() alone only mirrors the body state).
    const float startY = axed.getPosition().y;
    for (float t = 0.f; t < 0.5f; t += 0.05f) {
        world.Step(0.05f, 6, 2);
        axed.update(0.05f);
    }
    assert(axed.getPosition().y > startY);

    std::cout << "[PASSED] testBowserAxeCollapseKill" << std::endl;
}

void testBowserBreathesFire() {
    std::cout << "[RUNNING] testBowserBreathesFire..." << std::endl;

    b2World world = makeWorld();
    Bowser bowser({500.f, 416.f}, &world, LevelTheme::CASTLE);
    bowser.updateMarioPosition({100.f, 416.f}); // intruder on the left

    // Attacks are random (hop vs breathe); run long enough that at least
    // one breath happens and hands over a BowserFire.
    bool sawFire = false;
    for (int cycle = 0; cycle < 200 && !sawFire; ++cycle) {
        advance(bowser, 0.1f);
        for (auto& child : bowser.takePendingSpawns()) {
            assert(child->isEnemyProjectile());
            assert(static_cast<BowserFire*>(child.get())->isBowserFire());
            sawFire = true;
        }
    }
    assert(sawFire);

    std::cout << "[PASSED] testBowserBreathesFire" << std::endl;
}

void testBowserAxeEntity() {
    std::cout << "[RUNNING] testBowserAxeEntity..." << std::endl;

    b2World world = makeWorld();
    auto axe = EntityFactory::createFromTileCode('A', {300.f, 448.f}, &world,
                                                 LevelTheme::CASTLE);
    assert(axe != nullptr);
    assert(axe->isBowserAxe());
    assert(axe->getType() == Entity::EntityType::TERRAIN);

    // A body-less pickup: its overlap box is what Level watches.
    axe->update(0.1f);
    assert(axe->getBoundingBox().size == sf::Vector2f(32.f, 32.f));

    std::cout << "[PASSED] testBowserAxeEntity" << std::endl;
}

void testBridgeTileLoadsAndRemoves() {
    std::cout << "[RUNNING] testBridgeTileLoadsAndRemoves..." << std::endl;

    TileMap map;
    assert(map.loadFromFile("levels/level4.txt"));

    const auto bridge = map.findTiles('=');
    assert(bridge.size() == 13);

    // The bridge is solid, walkable terrain that can be demolished tile by tile.
    assert(map.isSolid(bridge.front().x, bridge.front().y));
    map.removeTile(bridge.front().x, bridge.front().y);
    assert(!map.isSolid(bridge.front().x, bridge.front().y));
    assert(map.findTiles('=').size() == 12);

    std::cout << "[PASSED] testBridgeTileLoadsAndRemoves" << std::endl;
}

int main() {
    std::cout << "=== Running Bowser Tests ===" << std::endl;
    testBowserCreation();
    testBowserFireballChips();
    testBowserStarIsHarmless();
    testBowserAxeCollapseKill();
    testBowserBreathesFire();
    testBowserAxeEntity();
    testBridgeTileLoadsAndRemoves();
    std::cout << "All Bowser tests PASSED successfully!" << std::endl;
    return 0;
}
