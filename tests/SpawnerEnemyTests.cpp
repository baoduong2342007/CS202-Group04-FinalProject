/**
 * @file SpawnerEnemyTests.cpp
 * @brief Unit tests for the Phase 3 spawner enemies: Bullet Bill + launcher,
 *        Lakitu (egg drops, respawn), Spiny egg (bounce + hatch), Spiny
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <box2d/box2d.h>

#include "entities/BulletBill.h"
#include "entities/BulletBillLauncher.h"
#include "entities/Lakitu.h"
#include "entities/SpinyEgg.h"
#include "entities/Spiny.h"
#include "entities/Mario.h"
#include "patterns/EntityFactory.h"
#include "physics/CollisionManager.h"

namespace {

b2World makeWorld() {
    return b2World(b2Vec2(0.f, 25.f));
}

void advance(Enemy& enemy, float seconds, float dt = 0.1f) {
    for (float t = 0.f; t < seconds; t += dt) {
        enemy.update(dt);
    }
}

void advance(Entity& entity, float seconds, float dt = 0.1f) {
    for (float t = 0.f; t < seconds; t += dt) {
        entity.update(dt);
    }
}

} // namespace

void testBulletBillCreation() {
    std::cout << "[RUNNING] testBulletBillCreation..." << std::endl;

    b2World world = makeWorld();
    BulletBill bullet({100.f, 100.f}, &world, LevelTheme::OVERWORLD, Direction::LEFT);

    assert(bullet.isBulletBill());
    assert(bullet.isFireproof());
    assert(bullet.getStompScore() == 200);
    assert(bullet.getDirection() == Direction::LEFT);

    // Level flight: constant horizontal speed, no vertical drift.
    bullet.update(0.1f);
    sf::Vector2f velocity = bullet.getVelocity();
    assert(std::abs(velocity.x + 240.f) < 1.f);
    assert(std::abs(velocity.y) < 0.1f);

    std::cout << "[PASSED] testBulletBillCreation" << std::endl;
}

void testBulletBillStompScore() {
    std::cout << "[RUNNING] testBulletBillStompScore..." << std::endl;

    b2World world = makeWorld();
    BulletBill bullet({100.f, 100.f}, &world, LevelTheme::OVERWORLD, Direction::RIGHT);

    Mario mario({100.f, 70.f}, {28.f, 30.f});
    mario.initPhysics(&world, b2_dynamicBody, {28.f, 30.f});
    const int initialScore = mario.getScore();

    assert(CollisionManager::defeatEnemy(bullet, DefeatCause::STOMP, &mario));
    assert(bullet.isDying());
    // Flat species price: exactly 200, never the airborne stomp chain.
    assert(mario.getScore() == initialScore + 200);

    std::cout << "[PASSED] testBulletBillStompScore" << std::endl;
}

void testLauncherSpawnsBullet() {
    std::cout << "[RUNNING] testLauncherSpawnsBullet..." << std::endl;

    b2World world = makeWorld();
    BulletBillLauncher launcher({400.f, 400.f}, &world, LevelTheme::OVERWORLD);

    assert(launcher.isBulletBillLauncher());
    assert(launcher.getType() == Entity::EntityType::TERRAIN);

    // Idle while nobody is in range.
    launcher.update(5.f);
    assert(launcher.takePendingSpawns().empty());

    // A player to the left is in range: the first bullet fires after the
    // interval, flying toward the player.
    launcher.updateMarioPosition({250.f, 400.f});
    advance(launcher, 3.f);
    auto spawns = launcher.takePendingSpawns();
    assert(spawns.size() == 1);
    assert(spawns.front()->isBulletBill());
    assert(static_cast<BulletBill*>(spawns.front().get())->getDirection() == Direction::LEFT);

    // Far away again: no more bullets.
    launcher.updateMarioPosition({2000.f, 400.f});
    advance(launcher, 5.f);
    assert(launcher.takePendingSpawns().empty());

    std::cout << "[PASSED] testLauncherSpawnsBullet" << std::endl;
}

void testLakituEggDropCycle() {
    std::cout << "[RUNNING] testLakituEggDropCycle..." << std::endl;

    b2World world = makeWorld();
    Lakitu lakitu({300.f, 64.f}, &world, LevelTheme::OVERWORLD);

    assert(lakitu.isLakitu());
    assert(lakitu.getStompScore() == 800);

    lakitu.updateMarioPosition({500.f, 400.f});

    // Cruise -> hide -> throw produces exactly one Spiny egg.
    advance(lakitu, 2.6f);
    assert(lakitu.getState() == Lakitu::State::HIDE);
    advance(lakitu, 1.1f);
    assert(lakitu.getState() == Lakitu::State::THROW);
    auto spawns = lakitu.takePendingSpawns();
    assert(spawns.size() == 1);
    assert(spawns.front()->isSpinyEgg());

    // The throw window closes and Lakitu keeps cruising.
    advance(lakitu, 0.5f);
    assert(lakitu.getState() == Lakitu::State::CRUISE);
    assert(lakitu.takePendingSpawns().empty());

    std::cout << "[PASSED] testLakituEggDropCycle" << std::endl;
}

void testLakituRespawnAfterStomp() {
    std::cout << "[RUNNING] testLakituRespawnAfterStomp..." << std::endl;

    b2World world = makeWorld();
    Lakitu lakitu({300.f, 64.f}, &world, LevelTheme::OVERWORLD);
    Mario mario({300.f, 20.f}, {28.f, 30.f});
    mario.initPhysics(&world, b2_dynamicBody, {28.f, 30.f});

    lakitu.updateMarioPosition({300.f, 20.f});

    assert(CollisionManager::defeatEnemy(lakitu, DefeatCause::STOMP, &mario));
    assert(lakitu.isDying());

    // The corpse falls; once it sinks far enough Lakitu returns to flight
    // height and can be stomped again.
    lakitu.update(0.1f);
    lakitu.getBody()->SetTransform(b2Vec2(lakitu.getBody()->GetPosition().x, 20.f), 0.f);
    advance(lakitu, 0.2f);
    assert(!lakitu.isDying());
    assert(lakitu.getState() == Lakitu::State::CRUISE);

    assert(CollisionManager::defeatEnemy(lakitu, DefeatCause::STOMP, &mario));

    std::cout << "[PASSED] testLakituRespawnAfterStomp" << std::endl;
}

void testSpinyEggHatchesViaFuse() {
    std::cout << "[RUNNING] testSpinyEggHatchesViaFuse..." << std::endl;

    b2World world = makeWorld();
    SpinyEgg egg({300.f, 100.f}, &world, LevelTheme::OVERWORLD, Direction::LEFT);

    assert(egg.isSpinyEgg());
    assert(!static_cast<Enemy&>(egg).canBeStomped());

    // No ground in this bare world: the fuse guarantees the hatch anyway.
    advance(egg, 4.2f);
    assert(egg.hasHatched());
    auto spawns = egg.takePendingSpawns();
    assert(spawns.size() == 1);
    assert(spawns.front()->isSpiny());

    std::cout << "[PASSED] testSpinyEggHatchesViaFuse" << std::endl;
}

void testSpinyUnstompable() {
    std::cout << "[RUNNING] testSpinyUnstompable..." << std::endl;

    b2World world = makeWorld();
    auto entity = EntityFactory::createFromTileCode('s', {100.f, 100.f}, &world,
                                                    LevelTheme::OVERWORLD);
    assert(entity != nullptr);
    assert(entity->isSpiny());

    Spiny* spiny = static_cast<Spiny*>(entity.get());
    assert(!spiny->canBeStomped());
    assert(spiny->getFacingDirection() == Direction::LEFT);

    // Fireball defeat still works (200 default).
    Mario mario({50.f, 100.f}, {28.f, 30.f});
    const int initialScore = mario.getScore();
    assert(CollisionManager::defeatEnemy(*spiny, DefeatCause::FIREBALL, &mario));
    assert(spiny->isDying());
    assert(mario.getScore() == initialScore + 200);

    std::cout << "[PASSED] testSpinyUnstompable" << std::endl;
}

void testFactoryTileCodes() {
    std::cout << "[RUNNING] testFactoryTileCodes..." << std::endl;

    b2World world = makeWorld();

    auto lakitu = EntityFactory::createFromTileCode('t', {100.f, 64.f}, &world,
                                                    LevelTheme::OVERWORLD);
    assert(lakitu && lakitu->isLakitu());

    auto launcher = EntityFactory::createFromTileCode('D', {100.f, 100.f}, &world,
                                                      LevelTheme::CASTLE);
    assert(launcher && launcher->isBulletBillLauncher());

    auto spinyByEnum = EntityFactory::createEnemy(EnemyType::SPINY, {100.f, 100.f},
                                                  &world, LevelTheme::OVERWORLD);
    assert(spinyByEnum && spinyByEnum->isSpiny());

    auto bulletByEnum = EntityFactory::createEnemy(EnemyType::BULLET_BILL, {100.f, 100.f},
                                                   &world, LevelTheme::OVERWORLD);
    assert(bulletByEnum && bulletByEnum->isBulletBill());

    std::cout << "[PASSED] testFactoryTileCodes" << std::endl;
}

int main() {
    std::cout << "=== Running Spawner Enemy Tests ===" << std::endl;
    testBulletBillCreation();
    testBulletBillStompScore();
    testLauncherSpawnsBullet();
    testLakituEggDropCycle();
    testLakituRespawnAfterStomp();
    testSpinyEggHatchesViaFuse();
    testSpinyUnstompable();
    testFactoryTileCodes();
    std::cout << "All Spawner Enemy tests PASSED successfully!" << std::endl;
    return 0;
}
