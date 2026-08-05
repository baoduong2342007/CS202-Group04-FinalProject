/**
 * @file MarioPhysicsTests.cpp
 * @author TV5 (Truyen)
 * @brief Regression tests for Mario grounding and jumping.
 */

#include <cmath>
#include <cstdlib>
#include <iostream>

#include <box2d/box2d.h>

#include "entities/Mario.h"
#include "physics/PhysicsEngine.h"

namespace {
constexpr float TIME_STEP = 1.0f / 60.0f;

bool check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return false;
    }
    return true;
}

b2Body* createPlatform(b2World& world) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(5.0f, 8.0f);
    b2Body* platform = world.CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(5.0f, 0.5f);
    platform->CreateFixture(&shape, 0.0f);
    return platform;
}

bool settleMarioOnPlatform(b2World& world, Mario& mario) {
    for (int step = 0; step < 360; ++step) {
        world.Step(TIME_STEP, 8, 3);
        mario.refreshGroundedState();
        if (mario.isGrounded()) {
            return true;
        }
    }
    return false;
}

bool testLandingRefreshesGroundingAndAllowsJump() {
    b2World world({0.0f, 25.0f});
    createPlatform(world);

    Mario mario({134.0f, 0.0f}, {32.0f, 32.0f});
    mario.initPhysics(&world, b2_dynamicBody, {32.0f, 32.0f});

    if (!check(settleMarioOnPlatform(world, mario),
               "Mario must become grounded after falling onto a static platform")) {
        return false;
    }

    mario.jump();
    mario.preparePhysics(TIME_STEP);

    return check(!mario.isGrounded(), "jump must clear grounded before the next physics step") &&
           check(mario.getBody()->GetLinearVelocity().y < -0.1f,
                 "grounded Mario must receive upward jump velocity");
}

bool testGroundingRecoversAfterTerrainRebuild() {
    b2World world({0.0f, 25.0f});
    b2Body* platform = createPlatform(world);

    Mario mario({134.0f, 0.0f}, {32.0f, 32.0f});
    mario.initPhysics(&world, b2_dynamicBody, {32.0f, 32.0f});

    if (!check(settleMarioOnPlatform(world, mario),
               "Mario must initially settle before the terrain rebuild")) {
        return false;
    }

    world.DestroyBody(platform);
    mario.refreshGroundedState();
    if (!check(!mario.isGrounded(),
               "removing terrain must not retain a stale grounded state")) {
        return false;
    }

    createPlatform(world);
    return check(settleMarioOnPlatform(world, mario),
                 "Mario must regain grounded state after rebuilt terrain contacts");
}
} // namespace

int main() {
    const bool success = testLandingRefreshesGroundingAndAllowsJump() &&
                         testGroundingRecoversAfterTerrainRebuild();
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
