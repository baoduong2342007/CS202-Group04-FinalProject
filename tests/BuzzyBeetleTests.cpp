/**
 * @file BuzzyBeetleTests.cpp
 * @brief Unit tests for the Buzzy Beetle: fireproof shell, kickable shell,
 *        and the SMB1-accurate warning-less wake-up
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <box2d/box2d.h>

#include "entities/BuzzyBeetle.h"
#include "entities/Koopa.h"
#include "entities/PiranhaPlant.h"
#include "patterns/EntityFactory.h"

namespace {

b2World makeWorld() {
    return b2World(b2Vec2(0.f, 25.f));
}



} // namespace

void testBuzzyCreation() {
    std::cout << "[RUNNING] testBuzzyCreation..." << std::endl;

    b2World world = makeWorld();
    auto entity = EntityFactory::createFromTileCode('b', {100.f, 100.f}, &world,
                                                    LevelTheme::UNDERGROUND);
    assert(entity != nullptr);
    assert(entity->getType() == Entity::EntityType::ENEMY);
    assert(entity->isEnemy());
    assert(entity->isKoopa());
    assert(entity->isBuzzyBeetle());

    BuzzyBeetle* buzzy = static_cast<BuzzyBeetle*>(entity.get());
    assert(buzzy->getState() == KoopaState::WALKING);
    // The Buzzy Beetle is a single-tile enemy (unlike the 32x48 Koopa).
    assert(buzzy->getSize() == sf::Vector2f(32.f, 32.f));

    std::cout << "[PASSED] testBuzzyCreation" << std::endl;
}

void testBuzzyFireproof() {
    std::cout << "[RUNNING] testBuzzyFireproof..." << std::endl;

    b2World world = makeWorld();
    BuzzyBeetle buzzy({100.f, 100.f}, &world, LevelTheme::OVERWORLD);
    Koopa koopa({100.f, 100.f}, &world, LevelTheme::OVERWORLD);

    // The hard shell blocks fireballs; CollisionManager's fireball path
    // consults this flag and bursts the shot without a defeat transaction.
    assert(buzzy.isFireproof());
    assert(!koopa.isFireproof());

    std::cout << "[PASSED] testBuzzyFireproof" << std::endl;
}

void testBuzzyShellAndKick() {
    std::cout << "[RUNNING] testBuzzyShellAndKick..." << std::endl;

    b2World world = makeWorld();
    BuzzyBeetle buzzy({100.f, 100.f}, &world, LevelTheme::OVERWORLD);

    buzzy.onStomp();
    assert(buzzy.getState() == KoopaState::SHELL_IDLE);
    assert(buzzy.isInShell());

    buzzy.kick(Direction::RIGHT);
    assert(buzzy.getState() == KoopaState::SHELL_SLIDING);
    assert(std::abs(buzzy.getVelocity().x - 240.f) < 0.1f);

    std::cout << "[PASSED] testBuzzyShellAndKick" << std::endl;
}

void testBuzzyWakeWithoutWarning() {
    std::cout << "[RUNNING] testBuzzyWakeWithoutWarning..." << std::endl;

    b2World world = makeWorld();
    BuzzyBeetle buzzy({100.f, 100.f}, &world, LevelTheme::OVERWORLD);

    buzzy.onStomp();
    assert(buzzy.getState() == KoopaState::SHELL_IDLE);

    // Sample the whole wake window: the Buzzy Beetle never shows the
    // legs-out warning frame, unlike the green Koopa.
    for (float t = 0.f; t < 7.f; t += 0.1f) {
        buzzy.update(0.1f);
        assert(buzzy.getState() != KoopaState::SHELL_WAKING);
    }
    assert(buzzy.getState() == KoopaState::WALKING);

    std::cout << "[PASSED] testBuzzyWakeWithoutWarning" << std::endl;
}

void testRedPiranhaPlantColor() {
    std::cout << "[RUNNING] testRedPiranhaPlantColor..." << std::endl;

    b2World world = makeWorld();

    auto red = EntityFactory::createFromTileCode('q', {100.f, 100.f}, &world,
                                                 LevelTheme::UNDERGROUND);
    assert(red != nullptr);
    assert(red->isPiranhaPlant());
    assert(static_cast<PiranhaPlant*>(red.get())->getColor() == PiranhaPlant::Color::RED);

    auto green = EntityFactory::createFromTileCode('p', {200.f, 100.f}, &world,
                                                   LevelTheme::UNDERGROUND);
    assert(green != nullptr);
    assert(static_cast<PiranhaPlant*>(green.get())->getColor() == PiranhaPlant::Color::GREEN);

    std::cout << "[PASSED] testRedPiranhaPlantColor" << std::endl;
}

int main() {
    std::cout << "=== Running Buzzy Beetle Tests ===" << std::endl;
    testBuzzyCreation();
    testBuzzyFireproof();
    testBuzzyShellAndKick();
    testBuzzyWakeWithoutWarning();
    testRedPiranhaPlantColor();
    std::cout << "All Buzzy Beetle tests PASSED successfully!" << std::endl;
    return 0;
}
