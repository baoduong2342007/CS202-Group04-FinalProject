/**
 * @file BlooperPodobooTests.cpp
 * @brief Unit tests for the Phase 2 environmental enemies:
 *        Blooper (unstompable homing squid) and Podoboo (invincible lava leap)
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <box2d/box2d.h>

#include "entities/Blooper.h"
#include "entities/Podoboo.h"
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

} // namespace

void testBlooperCreation() {
    std::cout << "[RUNNING] testBlooperCreation..." << std::endl;

    b2World world = makeWorld();
    auto entity = EntityFactory::createFromTileCode('l', {100.f, 100.f}, &world,
                                                    LevelTheme::UNDERWATER);
    assert(entity != nullptr);
    assert(entity->isEnemy());
    assert(entity->isBlooper());
    assert(!static_cast<Enemy*>(entity.get())->canBeStomped());

    Blooper* blooper = static_cast<Blooper*>(entity.get());
    assert(blooper->getBody() != nullptr);
    // The squid floats: gravity is disabled while alive.
    assert(std::abs(blooper->getBody()->GetGravityScale()) < 0.001f);

    std::cout << "[PASSED] testBlooperCreation" << std::endl;
}

void testBlooperDriftRiseCycle() {
    std::cout << "[RUNNING] testBlooperDriftRiseCycle..." << std::endl;

    b2World world = makeWorld();
    Blooper blooper({200.f, 200.f}, &world, LevelTheme::UNDERWATER);

    // RISE phase first: upward thrust, no horizontal drift.
    blooper.update(0.1f);
    assert(blooper.getState() == Blooper::State::RISE);
    sf::Vector2f velocity = blooper.getVelocity();
    assert(velocity.y < 0.f);
    assert(std::abs(velocity.x) < 0.1f);

    // After the rise window it switches to a sinking drift toward Mario.
    blooper.updateMarioPosition({600.f, 200.f});
    advance(blooper, 1.0f);
    assert(blooper.getState() == Blooper::State::DRIFT);
    velocity = blooper.getVelocity();
    assert(velocity.y > 0.f);
    assert(velocity.x > 0.f); // Mario is to the right

    // ... and the cycle returns to RISE.
    advance(blooper, 1.3f);
    assert(blooper.getState() == Blooper::State::RISE);

    std::cout << "[PASSED] testBlooperDriftRiseCycle" << std::endl;
}

void testBlooperFireballDefeat() {
    std::cout << "[RUNNING] testBlooperFireballDefeat..." << std::endl;

    b2World world = makeWorld();
    Blooper blooper({150.f, 150.f}, &world, LevelTheme::UNDERWATER);
    Mario mario({100.f, 150.f}, {28.f, 30.f});

    const int initialScore = mario.getScore();
    const bool defeated = CollisionManager::defeatEnemy(blooper, DefeatCause::FIREBALL, &mario);
    assert(defeated);
    assert(blooper.isDying());
    assert(mario.getScore() == initialScore + 200);

    std::cout << "[PASSED] testBlooperFireballDefeat" << std::endl;
}

void testPodobooCreation() {
    std::cout << "[RUNNING] testPodobooCreation..." << std::endl;

    b2World world = makeWorld();
    auto entity = EntityFactory::createFromTileCode('P', {300.f, 400.f}, &world,
                                                    LevelTheme::CASTLE);
    assert(entity != nullptr);
    assert(entity->isEnemy());
    assert(entity->isPodoboo());
    Enemy* podobooBase = static_cast<Enemy*>(entity.get());
    assert(podobooBase->isIndestructible());
    assert(!podobooBase->canBeStomped());

    std::cout << "[PASSED] testPodobooCreation" << std::endl;
}

void testPodobooLeapCycle() {
    std::cout << "[RUNNING] testPodobooLeapCycle..." << std::endl;

    b2World world = makeWorld();
    Podoboo podoboo({300.f, 400.f}, &world, LevelTheme::CASTLE);
    const float lavaY = podoboo.getPosition().y;

    // Waiting below the lava line before the first leap (spawn stagger 0.4 s
    // for this column, then a 1.2 s submerged pause).
    assert(!podoboo.isAirborne());

    // The leap rises well above the lava line (peak ~192 px at t≈0.69 s
    // after launch; sampled mid-flight)...
    advance(podoboo, 2.0f);
    assert(podoboo.isAirborne());
    assert(podoboo.getPosition().y < lavaY - 100.f);

    // ...falls back, submerges...
    advance(podoboo, 1.4f);
    assert(!podoboo.isAirborne());

    // ...and leaps again after the next pause.
    advance(podoboo, 1.2f);
    assert(podoboo.isAirborne());

    std::cout << "[PASSED] testPodobooLeapCycle" << std::endl;
}

void testPodobooIndestructible() {
    std::cout << "[RUNNING] testPodobooIndestructible..." << std::endl;

    b2World world = makeWorld();
    Podoboo podoboo({300.f, 400.f}, &world, LevelTheme::CASTLE);
    Mario mario({250.f, 400.f}, {28.f, 30.f});

    // Fireballs, star power, and shell kills all bounce off the transaction
    // guard: no defeat, no score, no death state.
    const int initialScore = mario.getScore();
    assert(!CollisionManager::defeatEnemy(podoboo, DefeatCause::FIREBALL, &mario));
    assert(!CollisionManager::defeatEnemy(podoboo, DefeatCause::STAR, &mario));
    assert(!CollisionManager::defeatEnemy(podoboo, DefeatCause::SHELL, &mario));
    assert(mario.getScore() == initialScore);
    assert(!podoboo.isDying());
    assert(podoboo.isActive());

    std::cout << "[PASSED] testPodobooIndestructible" << std::endl;
}

int main() {
    std::cout << "=== Running Blooper & Podoboo Tests ===" << std::endl;
    testBlooperCreation();
    testBlooperDriftRiseCycle();
    testBlooperFireballDefeat();
    testPodobooCreation();
    testPodobooLeapCycle();
    testPodobooIndestructible();
    std::cout << "All Blooper & Podoboo tests PASSED successfully!" << std::endl;
    return 0;
}
