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
#include "entities/FireBallPool.h"
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

bool testShortJumpVsLongJump() {
    // 1. Long jump test (jump key held)
    b2World worldLong({0.0f, 25.0f});
    createPlatform(worldLong);

    Mario marioLong({134.0f, 0.0f}, {32.0f, 32.0f});
    marioLong.initPhysics(&worldLong, b2_dynamicBody, {32.0f, 32.0f});
    settleMarioOnPlatform(worldLong, marioLong);

    marioLong.jump();
    marioLong.preparePhysics(TIME_STEP);
    worldLong.Step(TIME_STEP, 8, 3);

    float longJumpMinY = PhysicsEngine::metersToPixels(marioLong.getBody()->GetPosition().y);
    for (int step = 0; step < 60; ++step) {
        marioLong.preparePhysics(TIME_STEP); // jumpKeyReleased is false
        worldLong.Step(TIME_STEP, 8, 3);
        float currentY = PhysicsEngine::metersToPixels(marioLong.getBody()->GetPosition().y);
        longJumpMinY = std::min(longJumpMinY, currentY);
    }

    // 2. Short jump test (jump key released early while ascending)
    b2World worldShort({0.0f, 25.0f});
    createPlatform(worldShort);

    Mario marioShort({134.0f, 0.0f}, {32.0f, 32.0f});
    marioShort.initPhysics(&worldShort, b2_dynamicBody, {32.0f, 32.0f});
    settleMarioOnPlatform(worldShort, marioShort);

    marioShort.jump();
    marioShort.preparePhysics(TIME_STEP);
    worldShort.Step(TIME_STEP, 8, 3);

    // Release jump on the next frame while ascending
    marioShort.releaseJump();
    marioShort.preparePhysics(TIME_STEP);

    float shortJumpMinY = PhysicsEngine::metersToPixels(marioShort.getBody()->GetPosition().y);
    for (int step = 0; step < 60; ++step) {
        marioShort.preparePhysics(TIME_STEP);
        worldShort.Step(TIME_STEP, 8, 3);
        float currentY = PhysicsEngine::metersToPixels(marioShort.getBody()->GetPosition().y);
        shortJumpMinY = std::min(shortJumpMinY, currentY);
    }

    std::cout << "[TEST] Long jump min Y (pixels): " << longJumpMinY
              << ", Short jump min Y (pixels): " << shortJumpMinY << std::endl;

    // Peak height corresponds to smaller Y coordinate in screen space.
    // Short jump peak Y should be strictly greater (lower peak height) than long jump peak Y.
    return check(shortJumpMinY > longJumpMinY + 10.0f,
                 "Short jump peak height must be significantly lower than long jump peak height");
}

bool testSmallMarioTraversesOneBlockPassage() {
    b2World world({0.0f, 25.0f});
    
    // Create floor at Y = 8.0m (top surface at Y = 7.5m = 240px)
    b2BodyDef floorDef;
    floorDef.type = b2_staticBody;
    floorDef.position.Set(5.0f, 8.0f);
    b2Body* floor = world.CreateBody(&floorDef);
    b2PolygonShape floorShape;
    floorShape.SetAsBox(5.0f, 0.5f);
    floor->CreateFixture(&floorShape, 0.0f);

    // Create ceiling at Y = 6.0m (bottom surface at Y = 6.5m = 208px -> 1-block clearance = 32px)
    b2BodyDef ceilingDef;
    ceilingDef.type = b2_staticBody;
    ceilingDef.position.Set(5.0f, 6.0f);
    b2Body* ceiling = world.CreateBody(&ceilingDef);
    b2PolygonShape ceilingShape;
    ceilingShape.SetAsBox(5.0f, 0.5f);
    ceiling->CreateFixture(&ceilingShape, 0.0f);

    // Spawn Mario at Y = 220px (7.0m) inside the 1-block passage
    Mario mario({134.0f, 220.0f}, {28.0f, 30.0f});
    mario.initPhysics(&world, b2_dynamicBody, {28.0f, 30.0f});
    settleMarioOnPlatform(world, mario);

    float startX = PhysicsEngine::metersToPixels(mario.getBody()->GetPosition().x);
    mario.moveRight();
    for (int step = 0; step < 60; ++step) {
        mario.preparePhysics(TIME_STEP);
        world.Step(TIME_STEP, 8, 3);
        mario.refreshGroundedState();
        mario.update(TIME_STEP);
    }
    float endX = PhysicsEngine::metersToPixels(mario.getBody()->GetPosition().x);

    std::cout << "[TEST] One-block passage traversal start X: " << startX
              << ", end X: " << endX << std::endl;

    return check(endX > startX + 50.0f,
                 "Small Mario must move smoothly through 1-block-high passage without getting stuck");
}

bool testSuperMarioTraversesTwoBlockPassage() {
    b2World world({0.0f, 25.0f});
    
    // Create floor at Y = 8.0m (top surface at Y = 7.5m = 240px)
    b2BodyDef floorDef;
    floorDef.type = b2_staticBody;
    floorDef.position.Set(5.0f, 8.0f);
    b2Body* floor = world.CreateBody(&floorDef);
    b2PolygonShape floorShape;
    floorShape.SetAsBox(5.0f, 0.5f);
    floor->CreateFixture(&floorShape, 0.0f);

    // Create ceiling at Y = 5.0m (bottom surface at Y = 5.5m = 176px -> 2-block clearance = 64px)
    b2BodyDef ceilingDef;
    ceilingDef.type = b2_staticBody;
    ceilingDef.position.Set(5.0f, 5.0f);
    b2Body* ceiling = world.CreateBody(&ceilingDef);
    b2PolygonShape ceilingShape;
    ceilingShape.SetAsBox(5.0f, 0.5f);
    ceiling->CreateFixture(&ceilingShape, 0.0f);

    // Spawn Super Mario at Y = 200px inside the 2-block passage
    Mario mario({134.0f, 200.0f}, {32.0f, 64.0f});
    mario.initPhysics(&world, b2_dynamicBody, {32.0f, 64.0f});
    mario.setMarioState(MarioState::SUPER);
    settleMarioOnPlatform(world, mario);

    float startX = PhysicsEngine::metersToPixels(mario.getBody()->GetPosition().x);
    mario.moveRight();
    for (int step = 0; step < 60; ++step) {
        mario.preparePhysics(TIME_STEP);
        world.Step(TIME_STEP, 8, 3);
        mario.refreshGroundedState();
        mario.update(TIME_STEP);
    }
    float endX = PhysicsEngine::metersToPixels(mario.getBody()->GetPosition().x);

    std::cout << "[TEST] Two-block passage traversal start X: " << startX
              << ", end X: " << endX << std::endl;

    return check(endX > startX + 50.0f,
                 "Super Mario must move smoothly through 2-block-high passage without getting stuck");
}
bool testTimestepSubstepClamp() {
    b2World world({0.0f, 25.0f});
    float accumulator = 0.0f;
    // Pass a huge delta time spike of 1.0s (60 steps worth)
    const bool stepped = PhysicsEngine::update(world, 1.0f, accumulator);
    
    // Accumulator should drop excess time and reset to 0.0f on spike
    return check(stepped, "PhysicsEngine must step on large dt spike") &&
           check(std::abs(accumulator) < 0.001f, "Accumulator must reset to 0 after MAX_SUBSTEPS clamp");
}

bool testStarmanVsDamageGraceIndependence() {
    Mario mario({100.0f, 100.0f}, {28.0f, 30.0f});
    
    mario.activateStarman(5.0f);
    if (!check(mario.isStarInvincible(), "Starman activation must set isStarInvincible to true") ||
        !check(!mario.isDamageImmune(), "Starman activation must not set isDamageImmune to true") ||
        !check(mario.isInvincible(), "isInvincible must return true when Starman active")) {
        return false;
    }

    mario.update(6.0f); // Advance time past Starman duration
    if (!check(!mario.isStarInvincible(), "isStarInvincible must expire after duration")) {
        return false;
    }

    mario.activateDamageGrace(2.0f);
    if (!check(!mario.isStarInvincible(), "Damage grace activation must not set isStarInvincible") ||
        !check(mario.isDamageImmune(), "Damage grace activation must set isDamageImmune to true") ||
        !check(mario.isInvincible(), "isInvincible must return true when damage grace active")) {
        return false;
    }

    return true;
}

bool testFireBallPoolLimitAndMasks() {
    b2World world({0.0f, 25.0f});
    FireBallPool pool(2);
    
    FireBall* fb1 = pool.acquire({100.0f, 100.0f}, Direction::RIGHT, &world);
    FireBall* fb2 = pool.acquire({120.0f, 100.0f}, Direction::LEFT, &world);
    FireBall* fb3 = pool.acquire({140.0f, 100.0f}, Direction::RIGHT, &world);

    if (!check(fb1 != nullptr, "First fireball acquire must succeed") ||
        !check(fb2 != nullptr, "Second fireball acquire must succeed") ||
        !check(fb3 == nullptr, "Third fireball acquire must fail when pool capacity is 2") ||
        !check(pool.getActiveCount() == 2, "Active count must be exactly 2")) {
        return false;
    }

    return true;
}

bool testWalkingVsSprintingSeparatedByShift() {
    b2World world({0.0f, 25.0f});
    createPlatform(world);

    Mario marioWalk({134.0f, 0.0f}, {28.0f, 30.0f});
    marioWalk.initPhysics(&world, b2_dynamicBody, {28.0f, 30.0f});
    settleMarioOnPlatform(world, marioWalk);

    marioWalk.setRunning(false);
    marioWalk.moveRight();
    for (int step = 0; step < 60; ++step) {
        marioWalk.preparePhysics(TIME_STEP);
        world.Step(TIME_STEP, 8, 3);
    }
    float walkVx = std::abs(PhysicsEngine::metersToPixels(marioWalk.getBody()->GetLinearVelocity().x));

    Mario marioRun({134.0f, 0.0f}, {28.0f, 30.0f});
    marioRun.initPhysics(&world, b2_dynamicBody, {28.0f, 30.0f});
    settleMarioOnPlatform(world, marioRun);

    marioRun.setRunning(true);
    marioRun.moveRight();
    for (int step = 0; step < 60; ++step) {
        marioRun.preparePhysics(TIME_STEP);
        world.Step(TIME_STEP, 8, 3);
    }
    float runVx = std::abs(PhysicsEngine::metersToPixels(marioRun.getBody()->GetLinearVelocity().x));

    std::cout << "[TEST] Walk velocity: " << walkVx << " px/s, Run velocity: " << runVx << " px/s" << std::endl;

    return check(marioRun.isRunning(), "Holding shift must set isRunning to true") &&
           check(!marioWalk.isRunning(), "Releasing shift must keep isRunning as false") &&
           check(walkVx <= 181.0f, "Walking max speed must be capped at WALK_MAX_SPEED (180 px/s)") &&
           check(runVx > walkVx + 100.0f, "Sprinting with shift must achieve significantly higher velocity than walking");
}
} // namespace

int main() {
    const bool success = testLandingRefreshesGroundingAndAllowsJump() &&
                         testGroundingRecoversAfterTerrainRebuild() &&
                         testShortJumpVsLongJump() &&
                         testSmallMarioTraversesOneBlockPassage() &&
                         testSuperMarioTraversesTwoBlockPassage() &&
                         testTimestepSubstepClamp() &&
                         testStarmanVsDamageGraceIndependence() &&
                         testFireBallPoolLimitAndMasks() &&
                         testWalkingVsSprintingSeparatedByShift();
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
