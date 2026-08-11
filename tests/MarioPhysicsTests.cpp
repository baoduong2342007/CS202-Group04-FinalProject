/**
 * @file MarioPhysicsTests.cpp
 * @author TV5 (Truyen)
 * @brief Regression tests for Mario grounding and jumping.
 */

#include <cmath>
#include <cstdlib>
#include <iostream>

#include <box2d/box2d.h>

#include "core/AnimationSystem.h"
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

class MarioAnimationProbe final : public Mario {
public:
    using Mario::Mario;

    const std::string& currentAnimation() const {
        return m_animationSystem->getCurrentAnimationName();
    }
};

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

bool testUnderwaterSwimStrokeWorksInMidAir() {
    b2World world({0.0f, 8.0f});

    Mario mario({134.0f, 100.0f}, {32.0f, 32.0f});
    mario.initPhysics(&world, b2_dynamicBody, {32.0f, 32.0f});
    mario.setUnderwater(true);

    // Swimming must not depend on grounded state: a stroke is valid in open water.
    mario.jump();
    mario.preparePhysics(TIME_STEP);

    const float swimVelocity = mario.getBody()->GetLinearVelocity().y;
    return check(swimVelocity < -4.0f,
                 "underwater jump input must apply an upward swim impulse in mid-air");
}

bool testVineClimbUsesVerticalVelocityAndDetachesHorizontally() {
    b2World world({0.0f, 25.0f});

    Mario mario({134.0f, 160.0f}, {32.0f, 32.0f});
    mario.initPhysics(&world, b2_dynamicBody, {32.0f, 32.0f});

    mario.setVerticalIntent(-1.0f);
    mario.setClimbContext(true, 150.0f);
    mario.preparePhysics(TIME_STEP);

    const float climbVelocity = mario.getBody()->GetLinearVelocity().y;
    if (!check(mario.isClimbing(), "vertical input on a vine must enter climbing state") ||
        !check(mario.getBody()->GetGravityScale() == 0.0f,
               "climbing must disable gravity while attached to a vine") ||
        !check(climbVelocity < -2.0f,
               "upward vine input must produce deterministic upward velocity")) {
        return false;
    }

    mario.setVerticalIntent(1.0f);
    mario.setClimbContext(true, 150.0f);
    mario.preparePhysics(TIME_STEP);
    if (!check(mario.getBody()->GetLinearVelocity().y > 2.0f,
               "downward vine input must produce deterministic downward velocity")) {
        return false;
    }

    mario.setVerticalIntent(0.0f);
    mario.setClimbContext(true, 150.0f);
    mario.preparePhysics(TIME_STEP);
    if (!check(std::abs(mario.getBody()->GetLinearVelocity().y) < 0.01f,
               "released vertical input must hold Mario still on the vine")) {
        return false;
    }

    // Horizontal intent must win over vertical intent so the player can leave the vine.
    mario.setMoveIntent(1.0f);
    mario.setVerticalIntent(-1.0f);
    mario.setClimbContext(true, 150.0f);
    return check(!mario.isClimbing(),
                 "horizontal intent must prevent immediate reattachment to a vine") &&
           check(mario.getBody()->GetGravityScale() == 1.0f,
                 "leaving a vine must restore normal gravity");
}

bool testClimbAnimationDirectionSelection() {
    b2World world({0.0f, 25.0f});
    MarioAnimationProbe mario({134.0f, 160.0f}, {32.0f, 32.0f});
    mario.initPhysics(&world, b2_dynamicBody, {32.0f, 32.0f});

    mario.setVerticalIntent(-1.0f);
    mario.setClimbContext(true, 150.0f);
    mario.update(0.0f);
    if (!check(mario.currentAnimation() == "climb_up",
               "upward vine intent must select climb_up animation")) {
        return false;
    }

    mario.setVerticalIntent(1.0f);
    mario.setClimbContext(true, 150.0f);
    mario.update(0.0f);
    if (!check(mario.currentAnimation() == "climb_down",
               "downward vine intent must select climb_down animation")) {
        return false;
    }

    mario.setVerticalIntent(0.0f);
    mario.setClimbContext(true, 150.0f);
    mario.update(0.0f);
    if (!check(mario.currentAnimation() == "climb_idle",
               "released vertical intent must select climb_idle animation")) {
        return false;
    }

    mario.setFlagpoleSliding(true);
    mario.update(0.0f);
    return check(mario.currentAnimation() == "climb",
                 "flagpole sliding must retain the dedicated climb animation");
}
} // namespace

int main() {
    const bool success = testLandingRefreshesGroundingAndAllowsJump() &&
                         testGroundingRecoversAfterTerrainRebuild() &&
                         testShortJumpVsLongJump() &&
                         testSmallMarioTraversesOneBlockPassage() &&
                         testSuperMarioTraversesTwoBlockPassage() &&
                         testUnderwaterSwimStrokeWorksInMidAir() &&
                         testVineClimbUsesVerticalVelocityAndDetachesHorizontally() &&
                         testClimbAnimationDirectionSelection();
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
