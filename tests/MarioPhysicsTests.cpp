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
#include "items/FireFlower.h"
#include "items/Mushroom.h"
#include "physics/PhysicsEngine.h"
#include "entities/Goomba.h"
#include "entities/Koopa.h"
#include "physics/CollisionManager.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "core/ScoreRules.h"

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
    Mario mario({134.0f, 200.0f}, {28.0f, 60.0f});
    mario.setMarioState(MarioState::SUPER);
    mario.initPhysics(&world, b2_dynamicBody, {28.0f, 60.0f});
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

    mario.activateStarman(1.0f);
    mario.activateDamageGrace(2.0f);
    if (!check(mario.isStarInvincible(), "Starman activation must set isStarInvincible to true") ||
        !check(mario.isDamageImmune(), "Damage grace must coexist with Starman") ||
        !check(mario.isInvincible(), "isInvincible must return true when Starman active")) {
        return false;
    }

    mario.update(1.1f);
    if (!check(!mario.isStarInvincible(), "Starman must expire on its own clock") ||
        !check(mario.isDamageImmune(), "Starman expiry must preserve remaining damage grace")) {
        return false;
    }

    mario.update(1.0f);
    return check(!mario.isDamageImmune(),
                 "Damage grace must expire on its independent clock");
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



bool testIsolatedWorldAccumulators() {
    b2World world1({0.0f, 25.0f});
    b2World world2({0.0f, 25.0f});

    float accum1 = 0.0f;
    float accum2 = 0.0f;

    PhysicsEngine::update(world1, TIME_STEP * 0.5f, accum1);

    if (!check(accum1 > 0.0f, "World 1 accumulator must store time fraction")) return false;
    if (!check(accum2 == 0.0f, "World 2 accumulator must remain unaffected by World 1 update")) return false;

    PhysicsEngine::update(world2, TIME_STEP * 0.2f, accum2);
    if (!check(accum2 > 0.0f && accum2 < accum1, "World 2 accumulator must operate independently")) return false;

    return true;
}

bool testGrowthFootAnchorAndClearance() {
    b2World world({0.0f, 0.0f});
    Mario mario({100.0f, 100.0f}, {28.0f, 30.0f});
    mario.initPhysics(&world, b2_dynamicBody, {28.0f, 30.0f});
    mario.syncPhysics();

    float initialFootY = mario.getPosition().y + mario.getSize().y;
    Mushroom floorMushroom;
    floorMushroom.onCollect(mario);
    mario.syncPhysics();

    float newFootY = mario.getPosition().y + mario.getSize().y;
    if (!check(std::abs(newFootY - initialFootY) < 1.0f,
               "Growth to SUPER must preserve foot Y position on ground")) return false;

    // Test the production pickup path under a low ceiling.
    Mario smallMario({200.0f, 200.0f}, {28.0f, 30.0f});
    smallMario.initPhysics(&world, b2_dynamicBody, {28.0f, 30.0f});

    b2BodyDef ceilingDef;
    ceilingDef.type = b2_staticBody;
    ceilingDef.position.Set(PhysicsEngine::pixelsToMeters(214.0f),
                            PhysicsEngine::pixelsToMeters(185.0f));
    b2Body* ceiling = world.CreateBody(&ceilingDef);
    b2PolygonShape shape;
    shape.SetAsBox(PhysicsEngine::pixelsToMeters(16.0f), PhysicsEngine::pixelsToMeters(10.0f));
    ceiling->CreateFixture(&shape, 0.0f);

    Mushroom trappedMushroom;
    trappedMushroom.onCollect(smallMario);
    if (!check(smallMario.getMarioState() == MarioState::SMALL,
               "Mushroom pickup under a low ceiling must keep SMALL geometry") ||
        !check(smallMario.getPendingGrowthState() == MarioState::SUPER,
               "Blocked Mushroom pickup must retain SUPER as pending")) return false;

    const int scoreAfterPickup = smallMario.getScore();
    trappedMushroom.onCollect(smallMario);
    if (!check(smallMario.getScore() == scoreAfterPickup,
               "Repeated pickup contact must not award score twice")) return false;

    world.DestroyBody(ceiling);
    smallMario.update(0.0f);
    if (!check(smallMario.getMarioState() == MarioState::SUPER,
               "Pending Mushroom growth must apply once clearance opens") ||
        !check(!smallMario.hasPendingGrowth(),
               "Pending growth must clear after one transition")) return false;

    b2World flowerWorld({0.0f, 0.0f});
    Mario flowerMario({300.0f, 200.0f}, {28.0f, 30.0f});
    flowerMario.initPhysics(&flowerWorld, b2_dynamicBody, {28.0f, 30.0f});
    b2BodyDef flowerCeilingDef;
    flowerCeilingDef.type = b2_staticBody;
    flowerCeilingDef.position.Set(PhysicsEngine::pixelsToMeters(314.0f),
                                  PhysicsEngine::pixelsToMeters(185.0f));
    b2Body* flowerCeiling = flowerWorld.CreateBody(&flowerCeilingDef);
    flowerCeiling->CreateFixture(&shape, 0.0f);

    FireFlower flower;
    flower.onCollect(flowerMario);
    if (!check(flowerMario.getMarioState() == MarioState::FIRE,
               "Small Fire pickup must not force a large body") ||
        !check(flowerMario.getSize().y == 30.f,
               "Small Fire pickup must preserve Small Mario height") ||
        !check(!flowerMario.isSuperFireMario(),
               "Small Fire pickup must select the Small Fire form") ||
        !check(!flowerMario.hasPendingGrowth(),
               "Small Fire pickup must not defer a growth transition")) return false;

    flowerWorld.DestroyBody(flowerCeiling);
    flowerMario.update(0.0f);
    if (!check(flowerMario.getMarioState() == MarioState::FIRE,
               "Small Fire state must remain FIRE after clearance changes") ||
        !check(flowerMario.canShootFireBall(),
               "Small Fire Mario must be able to shoot")) return false;

    return true;
}

bool testDeathLifecycleAndDeterministicRespawn() {
    b2World world({0.0f, 25.0f});
    MarioAnimationProbe mario({100.0f, 100.0f}, {32.0f, 32.0f});
    mario.initPhysics(&world, b2_dynamicBody, {32.0f, 32.0f});

    int initialLives = mario.getLives();
    mario.setMarioState(MarioState::FIRE);
    assert(mario.tryStartFireBallShot());
    mario.setVerticalIntent(-1.0f);
    mario.setClimbContext(true, 116.0f);
    mario.setFlagpoleSliding(true);
    mario.loseLife();

    if (!check(mario.isDying(), "loseLife must set m_isDying to true") ||
        !check(mario.isCollisionLocked(),
               "dying Mario must be locked out of gameplay collisions") ||
        !check(mario.currentAnimation() == "death",
               "loseLife must activate the shared DEATH animation") ||
        !check(mario.getBody()->GetLinearVelocity().y < -1.0f,
               "loseLife must apply an upward death jump")) return false;
    if (!check(mario.getLives() == initialLives - 1, "loseLife must decrement lives by 1")) return false;
    if (!check(!mario.isRunning(), "loseLife must clear movement/run intent")) return false;

    const float deathStartY = mario.getPosition().y;
    mario.preparePhysics(TIME_STEP);
    if (!check(mario.getBody()->GetLinearVelocity().y < -1.0f,
               "preparePhysics must preserve the upward death jump")) return false;
    world.Step(TIME_STEP, 8, 3);
    mario.update(TIME_STEP);
    if (!check(mario.getPosition().y < deathStartY,
               "death jump must move Mario upward before the death clip ends")) return false;

    // Run through the end of the DEATH clip. Completion must still be held
    // back while the short post-clip fall tail is playing.
    for (int frame = 0; frame < 30; ++frame) {
        mario.preparePhysics(TIME_STEP);
        world.Step(TIME_STEP, 8, 3);
        mario.update(TIME_STEP);
    }
    const float deathClipEndY = mario.getPosition().y;
    if (!check(!mario.isDeathAnimationFinished(),
               "death completion must wait for the post-clip fall tail")) return false;

    for (int frame = 0; frame < 30 && !mario.isDeathAnimationFinished(); ++frame) {
        mario.preparePhysics(TIME_STEP);
        world.Step(TIME_STEP, 8, 3);
        mario.update(TIME_STEP);
    }
    if (!check(mario.isDeathAnimationFinished(),
               "death animation must publish its completion signal") ||
        !check(mario.getPosition().y > deathClipEndY,
               "Mario must fall visibly after the DEATH clip ends") ||
        !check(mario.currentAnimation() == "death",
               "the final DEATH frame must remain selected after the clip ends") ||
        !check(mario.isCollisionLocked(),
               "Mario must remain collision-locked until respawn/transition")) return false;

    mario.respawn({300.0f, 300.0f});
    if (!check(!mario.isDying(), "respawn must clear isDying flag")) return false;
    if (!check(mario.getMarioState() == MarioState::SMALL, "respawn must reset state to SMALL")) return false;
    if (!check(mario.getPosition().x == 300.0f && mario.getPosition().y == 300.0f,
               "respawn must reset position deterministically")) return false;
    if (!check(!mario.isClimbing() && !mario.isFlagpoleSliding(),
               "respawn must clear climb and flagpole state") ||
        !check(mario.getBody()->GetGravityScale() == 1.0f,
               "respawn must restore gravity") ||
        !check(mario.getFireCooldownRemaining() == 0.0f,
               "respawn must clear FireBall cooldown") ||
        !check(!mario.hasPendingGrowth(),
               "respawn must clear pending transforms")) return false;

    mario.update(0.6f);
    mario.loseLife();
    mario.update(0.6f);
    mario.respawn({300.0f, 300.0f});
    if (!check(mario.getPosition() == sf::Vector2f(300.0f, 300.0f),
               "second respawn must produce the same clean state")) return false;

    return true;
}

bool testCollisionManagerDefeatCausesAndSingleScore() {
    b2World world({0.0f, 0.0f});
    Mario mario({0.0f, 0.0f}, {32.0f, 32.0f});
    mario.initPhysics(&world, b2_dynamicBody, {32.0f, 32.0f});

    Goomba goomba1({100.0f, 100.0f}, &world);
    int initialScore = mario.getScore();

    bool defeat1 = CollisionManager::defeatEnemy(goomba1, DefeatCause::STOMP, &mario);
    if (!check(defeat1, "First stomp attempt must return true")) return false;
    if (!check(mario.getScore() == initialScore + 100, "STOMP cause must award 100 points")) return false;

    bool defeat2 = CollisionManager::defeatEnemy(goomba1, DefeatCause::STOMP, &mario);
    if (!check(!defeat2, "Second defeat attempt on dying victim must return false (idempotent)")) return false;
    if (!check(mario.getScore() == initialScore + 100, "Second defeat attempt must NOT award points twice")) return false;

    Goomba goomba2({150.0f, 150.0f}, &world);
    bool defeatFire = CollisionManager::defeatEnemy(goomba2, DefeatCause::FIREBALL, &mario);
    if (!check(defeatFire, "Fireball defeat must return true")) return false;
    if (!check(mario.getScore() == initialScore + 100 + 200, "FIREBALL cause must award 200 points")) return false;

    return true;
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
                         testTimestepSubstepClamp() &&
                         testStarmanVsDamageGraceIndependence() &&
                         testWalkingVsSprintingSeparatedByShift() &&
                         testIsolatedWorldAccumulators() &&
                         testGrowthFootAnchorAndClearance() &&
                         testDeathLifecycleAndDeterministicRespawn() &&
                         testCollisionManagerDefeatCausesAndSingleScore() &&
                         testUnderwaterSwimStrokeWorksInMidAir() &&
                         testVineClimbUsesVerticalVelocityAndDetachesHorizontally() &&
                         testClimbAnimationDirectionSelection();
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
