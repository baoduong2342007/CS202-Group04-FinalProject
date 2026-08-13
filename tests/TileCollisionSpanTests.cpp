/**
 * @file TileCollisionSpanTests.cpp
 * @author TV5 (Truyen)
 * @brief Regression tests for merged tile collision spans.
 */

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <box2d/box2d.h>

#include "level/TileCollisionSpans.h"
#include "level/TileSemantics.h"
#include "physics/TileContactResolver.h"
#include "entities/Mario.h"
#include "entities/QuestionBlock.h"
#include "level/TileMap.h"
#include "physics/ContactListener.h"
#include "physics/PhysicsEngine.h"

namespace {
constexpr float PIXELS_PER_METER = 30.0f;
constexpr float TILE_SIZE_PIXELS = 32.0f;
constexpr float TILE_SIZE_METERS = TILE_SIZE_PIXELS / PIXELS_PER_METER;
constexpr float TIME_STEP = 1.0f / 60.0f;

bool isSolid(char tile) {
    return tile == '0' || tile == 'B';
}

bool check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return false;
    }
    return true;
}

void createSpanBodies(b2World& world, const std::vector<TileCollisionSpan>& spans) {
    for (const TileCollisionSpan& span : spans) {
        const float widthMeters = static_cast<float>(span.length) * TILE_SIZE_METERS;

        b2BodyDef bodyDef;
        bodyDef.type = b2_staticBody;
        bodyDef.position.Set(
            static_cast<float>(span.column) * TILE_SIZE_METERS + widthMeters / 2.0f,
            static_cast<float>(span.row) * TILE_SIZE_METERS + TILE_SIZE_METERS / 2.0f
        );
        bodyDef.userData.pointer = TileMap::TILE_USERDATA_FLAG |
                                   (static_cast<uintptr_t>(span.row) << 16) |
                                   static_cast<uintptr_t>(span.column);
        b2Body* body = world.CreateBody(&bodyDef);

        b2PolygonShape shape;
        shape.SetAsBox(widthMeters / 2.0f, TILE_SIZE_METERS / 2.0f);
        body->CreateFixture(&shape, 0.0f);
    }
}

float simulateRightSpam(const std::vector<TileCollisionSpan>& spans) {
    b2World world({0.0f, 25.0f});
    createSpanBodies(world, spans);

    b2BodyDef marioDef;
    marioDef.type = b2_dynamicBody;
    marioDef.fixedRotation = true;
    marioDef.position.Set(3.5f * TILE_SIZE_METERS, -TILE_SIZE_METERS / 2.0f);
    b2Body* mario = world.CreateBody(&marioDef);

    b2PolygonShape marioShape;
    marioShape.SetAsBox(TILE_SIZE_METERS / 2.0f, TILE_SIZE_METERS / 2.0f);
    b2FixtureDef marioFixture;
    marioFixture.shape = &marioShape;
    marioFixture.density = 1.0f;
    marioFixture.friction = 0.0f;
    mario->CreateFixture(&marioFixture);

    for (int frame = 0; frame < 2'000; ++frame) {
        const float currentVxPixels = mario->GetLinearVelocity().x * PIXELS_PER_METER;
        const bool rightPressed = (frame % 2) == 0;
        const float nextVxPixels = rightPressed
                                       ? std::min(180.0f, currentVxPixels + 15.0f)
                                       : std::max(0.0f, currentVxPixels - 1100.0f * TIME_STEP);
        mario->SetLinearVelocity({nextVxPixels / PIXELS_PER_METER, mario->GetLinearVelocity().y});
        world.Step(TIME_STEP, 8, 3);
    }

    return mario->GetPosition().x * PIXELS_PER_METER;
}

void settleMario(b2World& world, Mario& mario, int frames = 10) {
    for (int frame = 0; frame < frames; ++frame) {
        world.Step(TIME_STEP, 8, 3);
        mario.refreshGroundedState();
        mario.update(TIME_STEP);
    }
}

float tapMoveMario(b2World& world, Mario& mario, float direction, int frames) {
    const float startX = mario.getPosition().x;
    for (int frame = 0; frame < frames; ++frame) {
        if ((frame % 2) == 0) {
            mario.setMoveIntent(direction);
        } else {
            mario.stopMoving();
        }
        mario.preparePhysics(TIME_STEP);
        world.Step(TIME_STEP, 8, 3);
        mario.refreshGroundedState();
        mario.update(TIME_STEP);
    }
    return mario.getPosition().x - startX;
}

void createAlternatingBlockCourse(
    b2World& world,
    std::vector<std::unique_ptr<QuestionBlock>>& questionBlocks,
    int startColumn,
    int length,
    int row
) {
    std::vector<TileCollisionSpan> brickSpans;
    for (int offset = 0; offset < length; ++offset) {
        const int column = startColumn + offset;
        if ((offset % 2) == 0) {
            brickSpans.push_back({column, row, 1});
        } else {
            questionBlocks.push_back(std::make_unique<QuestionBlock>(
                sf::Vector2f(static_cast<float>(column) * TILE_SIZE_PIXELS,
                             static_cast<float>(row) * TILE_SIZE_PIXELS),
                &world));
        }
    }
    createSpanBodies(world, brickSpans);
}

struct MarioTapRun {
    float displacement = 0.f;
    bool grounded = false;
    float jumpVelocity = 0.f;
};

MarioTapRun runMixedBlockTapCourse(float direction) {
    b2World world({0.f, 25.f});
    TileMap tileMap;
    ContactListener listener(tileMap);
    world.SetContactListener(&listener);

    std::vector<std::unique_ptr<QuestionBlock>> questionBlocks;
    constexpr int row = 6;
    constexpr int courseStart = 0;
    constexpr int courseLength = 100;
    createAlternatingBlockCourse(world, questionBlocks, courseStart, courseLength, row);

    const int startColumn = direction > 0.f ? 10 : 70;
    Mario mario(
        {static_cast<float>(startColumn) * TILE_SIZE_PIXELS + 2.f,
         static_cast<float>(row) * TILE_SIZE_PIXELS - 30.f},
        {28.f, 30.f});
    mario.initPhysics(&world, b2_dynamicBody, {28.f, 30.f});
    settleMario(world, mario);

    MarioTapRun result;
    result.displacement = tapMoveMario(world, mario, direction, 12'000);
    result.grounded = mario.isGrounded();
    mario.jump();
    mario.preparePhysics(TIME_STEP);
    result.jumpVelocity = PhysicsEngine::metersToPixels(
        mario.getBody()->GetLinearVelocity().y);
    return result;
}

bool testSpanLayout() {
    const std::vector<std::string> grid{
        "000.00",
        ".0...."
    };
    const std::vector<TileCollisionSpan> spans = buildHorizontalTileCollisionSpans(grid, isSolid);

    return check(spans.size() == 3, "gaps must split a row into independent collision spans") &&
           check(spans[0].column == 0 && spans[0].row == 0 && spans[0].length == 3,
                 "first solid run must become one span") &&
           check(spans[1].column == 4 && spans[1].row == 0 && spans[1].length == 2,
                 "second solid run must become one span") &&
           check(spans[2].column == 1 && spans[2].row == 1 && spans[2].length == 1,
                 "rows must remain independent");
}

bool testDestroyedTileSplitsSpan() {
    std::vector<std::string> grid{"00000"};
    grid[0][2] = '.';
    const std::vector<TileCollisionSpan> spans = buildHorizontalTileCollisionSpans(grid, isSolid);

    return check(spans.size() == 2, "a destroyed middle tile must split its collision span") &&
           check(spans[0].column == 0 && spans[0].length == 2,
                 "left terrain must remain solid") &&
           check(spans[1].column == 3 && spans[1].length == 2,
                 "right terrain must remain solid");
}

bool testRightSpamCrossesTileBoundaries() {
    const std::vector<std::string> grid{std::string(80, '0')};
    const std::vector<TileCollisionSpan> spans = buildHorizontalTileCollisionSpans(grid, isSolid);
    const float endPositionPixels = simulateRightSpam(spans);

    return check(spans.size() == 1, "a flat floor must have no internal vertical seams") &&
           check(endPositionPixels > 300.0f,
                 "right spam must cross many former tile boundaries without becoming stuck");
}

bool testMarioLeavesQuestionBlockToTheLeft() {
    b2World world({0.0f, 25.0f});
    // Exact level0 row: B ? B. B tiles are TileMap span bodies and '?' is a
    // separate QuestionBlock entity body.
    createSpanBodies(world, {{3, 6, 1}, {5, 6, 1}});
    QuestionBlock questionBlock({128.0f, 192.0f}, &world);

    Mario mario({130.0f, 162.0f}, {28.0f, 30.0f});
    mario.initPhysics(&world, b2_dynamicBody, {28.0f, 30.0f});

    float minimumVerticalVelocity = 0.0f;
    for (int frame = 0; frame < 120; ++frame) {
        mario.moveLeft();
        mario.preparePhysics(TIME_STEP);
        world.Step(TIME_STEP, 8, 3);
        mario.refreshGroundedState();
        mario.update(TIME_STEP);

        if (frame > 5 && frame < 55) {
            minimumVerticalVelocity = std::min(
                minimumVerticalVelocity,
                PhysicsEngine::metersToPixels(
                    mario.getBody()->GetLinearVelocity().y));
        }
    }

    const float endX = PhysicsEngine::metersToPixels(mario.getBody()->GetPosition().x);
    return check(endX < 90.0f,
                 "Mario must leave the B-question-B row toward the left") &&
           check(minimumVerticalVelocity > -1.0f,
                 "The left block seam must not create an upward ghost impulse");
}

bool testMarioTapSpamCrossesMixedBlockSeamsInBothDirections() {
    const MarioTapRun right = runMixedBlockTapCourse(1.f);
    const MarioTapRun left = runMixedBlockTapCourse(-1.f);

    return check(right.displacement > 250.f,
                 "Mario tap-spam must cross B/question seams to the right") &&
           check(left.displacement < -250.f,
                 "Mario tap-spam must cross B/question seams to the left") &&
           check(right.grounded && left.grounded,
                 "Mario must remain grounded while traversing a mixed block row") &&
           check(right.jumpVelocity < -100.f && left.jumpVelocity < -100.f,
                 "Mario must retain jump ability after crossing mixed block seams");
}

bool testMarioSeamFilterKeepsHigherWallSolid() {
    b2World world({0.f, 25.f});
    TileMap tileMap;
    ContactListener listener(tileMap);
    world.SetContactListener(&listener);

    // Ground top is y=192. The block at column 4 starts one row higher, so
    // it is a genuine wall/step and must not be treated as a walkable seam.
    createSpanBodies(world, {{0, 6, 16}, {4, 5, 1}});
    Mario mario({2.f, 162.f}, {28.f, 30.f});
    mario.initPhysics(&world, b2_dynamicBody, {28.f, 30.f});
    settleMario(world, mario);

    const float displacement = tapMoveMario(world, mario, 1.f, 12'000);
    return check(displacement < 110.f,
                 "a one-tile-higher wall must remain solid to Mario");
}

bool testMarioSeamFilterPreservesQuestionBlockCeiling() {
    b2World world({0.f, 0.f});
    TileMap tileMap;
    ContactListener listener(tileMap);
    world.SetContactListener(&listener);

    QuestionBlock questionBlock({96.f, 128.f}, &world);
    Mario mario({98.f, 180.f}, {28.f, 30.f});
    mario.initPhysics(&world, b2_dynamicBody, {28.f, 30.f});
    mario.setVelocity({0.f, -300.f});

    float minimumTopY = mario.getPosition().y;
    for (int frame = 0; frame < 90; ++frame) {
        mario.preparePhysics(TIME_STEP);
        world.Step(TIME_STEP, 8, 3);
        mario.refreshGroundedState();
        mario.update(TIME_STEP);
        minimumTopY = std::min(minimumTopY, mario.getPosition().y);
    }

    return check(minimumTopY > 120.f,
                 "Mario jumping from below must remain blocked by a question block");
}

bool testMarioSeamFilterDoesNotBridgeAPlatformEdge() {
    b2World world({0.f, 25.f});
    TileMap tileMap;
    ContactListener listener(tileMap);
    world.SetContactListener(&listener);

    createSpanBodies(world, {{0, 6, 2}});
    Mario mario({2.f, 162.f}, {28.f, 30.f});
    mario.initPhysics(&world, b2_dynamicBody, {28.f, 30.f});
    settleMario(world, mario);

    for (int frame = 0; frame < 90; ++frame) {
        mario.moveRight();
        mario.preparePhysics(TIME_STEP);
        world.Step(TIME_STEP, 8, 3);
        mario.refreshGroundedState();
        mario.update(TIME_STEP);
    }

    return check(!mario.isGrounded() && mario.getPosition().y > 220.f,
                 "Mario must fall after walking beyond a platform edge");
}

bool testCeilingContactTargetsOwningTile() {
    constexpr float tileSize = 32.0f;

    const TileContactCoordinate exactEdge =
        resolveCeilingTileContact(128.0f, 96.0f, tileSize);
    const TileContactCoordinate solverSlop =
        resolveCeilingTileContact(128.10f, 96.10f, tileSize);

    return check(exactEdge.column == 3 && exactEdge.row == 2,
                 "a contact on a tile's bottom edge must target that ceiling tile") &&
           check(solverSlop.column == 3 && solverSlop.row == 2,
                 "small Box2D contact slop must not select the row below the ceiling");
}

bool testGroundAndBrickSemantics() {
    return check(
               !TileSemantics::isBreakable('0'),
               "ground tile '0' must not be breakable"
           ) &&
           check(
               TileSemantics::isBreakable('B'),
               "brick tile 'B' must be breakable"
           );
}

} // namespace

int main() {
    const bool success = testGroundAndBrickSemantics() &&
                            testSpanLayout() &&
                            testDestroyedTileSplitsSpan() &&
                            testRightSpamCrossesTileBoundaries() &&
                            testMarioLeavesQuestionBlockToTheLeft() &&
                            testMarioTapSpamCrossesMixedBlockSeamsInBothDirections() &&
                            testMarioSeamFilterKeepsHigherWallSolid() &&
                            testMarioSeamFilterPreservesQuestionBlockCeiling() &&
                            testMarioSeamFilterDoesNotBridgeAPlatformEdge() &&
                            testCeilingContactTargetsOwningTile();

    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
