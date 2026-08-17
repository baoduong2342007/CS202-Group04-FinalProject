/**
 * @file HammerBroTests.cpp
 * @brief Unit tests for the Phase 4 projectile system: HammerBro (1000-point
 *        flat reward, hammer bursts) plus the Hammer and BowserFire hazards
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <box2d/box2d.h>

#include "entities/HammerBro.h"
#include "entities/Hammer.h"
#include "entities/BowserFire.h"
#include "entities/Mario.h"
#include "patterns/EntityFactory.h"
#include "physics/CollisionManager.h"

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

void testHammerBroCreation() {
    std::cout << "[RUNNING] testHammerBroCreation..." << std::endl;

    b2World world = makeWorld();
    auto entity = EntityFactory::createFromTileCode('n', {100.f, 100.f}, &world,
                                                    LevelTheme::CASTLE);
    assert(entity != nullptr);
    assert(entity->isEnemy());
    assert(entity->isHammerBro());

    HammerBro* bro = static_cast<HammerBro*>(entity.get());
    assert(bro->getState() == HammerBro::State::PATROL);
    assert(bro->getSize() == sf::Vector2f(32.f, 48.f));
    assert(bro->getStompScore() == 1000);

    std::cout << "[PASSED] testHammerBroCreation" << std::endl;
}

void testHammerBroFlatThousandEveryCause() {
    std::cout << "[RUNNING] testHammerBroFlatThousandEveryCause..." << std::endl;

    b2World world = makeWorld();

    // Stomp pays the flat species price, not the airborne chain.
    HammerBro stomped({100.f, 100.f}, &world, LevelTheme::CASTLE);
    Mario mario({100.f, 60.f}, {28.f, 30.f});
    mario.initPhysics(&world, b2_dynamicBody, {28.f, 30.f});
    const int initialScore = mario.getScore();

    assert(CollisionManager::defeatEnemy(stomped, DefeatCause::STOMP, &mario));
    assert(stomped.isDying());
    assert(mario.getScore() == initialScore + 1000);

    // Fireball pays the same flat price through the species override.
    HammerBro shot({200.f, 100.f}, &world, LevelTheme::CASTLE);
    const int beforeFireball = mario.getScore();
    assert(CollisionManager::defeatEnemy(shot, DefeatCause::FIREBALL, &mario));
    assert(mario.getScore() == beforeFireball + 1000);

    std::cout << "[PASSED] testHammerBroFlatThousandEveryCause" << std::endl;
}

void testHammerBroThrowsBursts() {
    std::cout << "[RUNNING] testHammerBroThrowsBursts..." << std::endl;

    b2World world = makeWorld();
    HammerBro bro({300.f, 100.f}, &world, LevelTheme::CASTLE);
    bro.updateMarioPosition({100.f, 100.f}); // player to the left

    // The first attack fires after the initial cooldown (2 s); the first
    // hammer leaves 0.25 s into the throw window.
    advance(bro, 2.4f);
    assert(bro.getState() == HammerBro::State::THROW);

    auto hammers = bro.takePendingSpawns();
    assert(!hammers.empty());
    for (const auto& hammer : hammers) {
        assert(hammer->isEnemyProjectile());
        assert(!hammer->isFireBall());
        assert(static_cast<Hammer*>(hammer.get())->getVelocity().x < 0.f);
        assert(static_cast<Hammer*>(hammer.get())->getVelocity().y < 0.f);
    }

    // The throw window closes and the patrol resumes.
    advance(bro, 0.7f);
    assert(bro.getState() == HammerBro::State::PATROL);

    std::cout << "[PASSED] testHammerBroThrowsBursts" << std::endl;
}

void testHammerArcAndSpin() {
    std::cout << "[RUNNING] testHammerArcAndSpin..." << std::endl;

    b2World world = makeWorld();
    Hammer hammer({300.f, 100.f}, &world, Direction::LEFT);

    assert(hammer.isEnemyProjectile());
    assert(hammer.getType() == Entity::EntityType::PROJECTILE);

    // Launched upward on an arc toward the left.
    sf::Vector2f velocity = hammer.getVelocity();
    assert(velocity.x < 0.f);
    assert(velocity.y < 0.f);

    // Gravity bends the arc downward over time (the world must be stepped;
    // update() alone only mirrors the body state).
    for (float t = 0.f; t < 1.f; t += 0.05f) {
        world.Step(0.05f, 6, 2);
        hammer.update(0.05f);
    }
    assert(hammer.getVelocity().y > 0.f);

    // The 4 s lifetime guard removes it eventually.
    advance(hammer, 3.2f);
    assert(hammer.shouldRemove());

    std::cout << "[PASSED] testHammerArcAndSpin" << std::endl;
}

void testBowserFireWave() {
    std::cout << "[RUNNING] testBowserFireWave..." << std::endl;

    b2World world = makeWorld();
    BowserFire fire({500.f, 200.f}, &world, LevelTheme::CASTLE, Direction::LEFT);

    assert(fire.isEnemyProjectile());
    assert(fire.isBowserFire());
    assert(!fire.isFireBall());

    // Travels straight left at constant speed...
    fire.update(0.1f);
    sf::Vector2f velocity = fire.getVelocity();
    assert(std::abs(velocity.x + 180.f) < 1.f);
    assert(std::abs(velocity.y) < 0.1f);

    // ...while its altitude oscillates around the launch row.
    const float baseY = fire.getPosition().y;
    float minY = baseY;
    float maxY = baseY;
    for (float t = 0.f; t < 1.f; t += 0.05f) {
        fire.update(0.05f);
        minY = std::min(minY, fire.getPosition().y);
        maxY = std::max(maxY, fire.getPosition().y);
    }
    assert(maxY - minY > 6.f); // subtle natural wave swing, 6 px amplitude
    assert(maxY - minY < 20.f);

    std::cout << "[PASSED] testBowserFireWave" << std::endl;
}

int main() {
    std::cout << "=== Running Hammer Bro & Projectile Tests ===" << std::endl;
    testHammerBroCreation();
    testHammerBroFlatThousandEveryCause();
    testHammerBroThrowsBursts();
    testHammerArcAndSpin();
    testBowserFireWave();
    std::cout << "All Hammer Bro & Projectile tests PASSED successfully!" << std::endl;
    return 0;
}
