/**
 * @file SpringboardTests.cpp
 * @author TV1 (Dương)
 * @brief Unit tests for Springboard / Trampoline mechanics and animation states
 */

#include <cassert>
#include <iostream>
#include <memory>
#include <box2d/box2d.h>

#include "entities/Springboard.h"
#include "entities/Mario.h"
#include "patterns/EntityFactory.h"
#include "physics/PhysicsEngine.h"

void testSpringboardCreation() {
    std::cout << "[RUNNING] testSpringboardCreation..." << std::endl;

    b2Vec2 gravity(0.f, 10.f);
    b2World world(gravity);

    auto spring = EntityFactory::createFromTileCode('J', {100.f, 200.f}, &world, LevelTheme::OVERWORLD);
    assert(spring != nullptr);
    assert(spring->getType() == Entity::EntityType::SPRINGBOARD);
    assert(spring->isSpringboard());

    Springboard* board = static_cast<Springboard*>(spring.get());
    assert(board->getState() == Springboard::State::IDLE);

    std::cout << "[PASSED] testSpringboardCreation" << std::endl;
}

void testSpringboardStateCycleAndBounce() {
    std::cout << "[RUNNING] testSpringboardStateCycleAndBounce..." << std::endl;

    b2Vec2 gravity(0.f, 10.f);
    b2World world(gravity);

    Springboard spring({100.f, 200.f}, LevelTheme::OVERWORLD);
    spring.initPhysics(&world, b2_staticBody, {32.f, 32.f});

    Mario mario({100.f, 160.f}, {28.f, 30.f});
    mario.initPhysics(&world, b2_dynamicBody, {28.f, 30.f});

    // 1. Normal Bounce (without holding jump)
    spring.triggerSpring(mario, false);
    assert(spring.getState() == Springboard::State::COMPRESSING);

    // Verify Mario's launch velocity
    b2Body* marioBody = mario.getBody();
    assert(marioBody != nullptr);
    float vy = marioBody->GetLinearVelocity().y;
    float expectedVy = -PhysicsEngine::pixelsToMeters(Springboard::BOUNCE_NORMAL_SPEED);
    assert(std::abs(vy - expectedVy) < 0.01f);

    // Update through compression phase (0.10s)
    spring.update(0.11f);
    assert(spring.getState() == Springboard::State::LAUNCHING);

    // Update through launching phase (0.12s)
    spring.update(0.13f);
    assert(spring.getState() == Springboard::State::COOLDOWN);

    // Update through cooldown phase (0.08s)
    spring.update(0.09f);
    assert(spring.getState() == Springboard::State::IDLE);

    // 2. Super Spring Jump (holding jump key)
    spring.triggerSpring(mario, true);
    assert(spring.getState() == Springboard::State::COMPRESSING);

    float superVy = marioBody->GetLinearVelocity().y;
    float expectedSuperVy = -PhysicsEngine::pixelsToMeters(Springboard::BOUNCE_SUPER_SPEED);
    assert(std::abs(superVy - expectedSuperVy) < 0.01f);

    std::cout << "[PASSED] testSpringboardStateCycleAndBounce" << std::endl;
}

int main() {
    std::cout << "=== Running Springboard Tests ===" << std::endl;
    testSpringboardCreation();
    testSpringboardStateCycleAndBounce();
    std::cout << "All Springboard tests PASSED successfully!" << std::endl;
    return 0;
}
