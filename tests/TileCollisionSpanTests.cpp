/**
 * @file TileCollisionSpanTests.cpp
 * @author TV5 (Truyen)
 * @brief Regression tests for merged tile collision spans.
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <box2d/box2d.h>

#include "level/TileCollisionSpans.h"
#include "level/TileSemantics.h"
#include "physics/TileContactResolver.h"

namespace {
constexpr float PIXELS_PER_METER = 30.0f;
constexpr float TILE_SIZE_PIXELS = 32.0f;
constexpr float TILE_SIZE_METERS = TILE_SIZE_PIXELS / PIXELS_PER_METER;
constexpr float TIME_STEP = 1.0f / 60.0f;

bool isSolid(char tile) {
    return tile == '1' || tile == 'B';
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

bool testSpanLayout() {
    const std::vector<std::string> grid{
        "111.11",
        ".1...."
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
    std::vector<std::string> grid{"11111"};
    grid[0][2] = '.';
    const std::vector<TileCollisionSpan> spans = buildHorizontalTileCollisionSpans(grid, isSolid);

    return check(spans.size() == 2, "a destroyed middle tile must split its collision span") &&
           check(spans[0].column == 0 && spans[0].length == 2,
                 "left terrain must remain solid") &&
           check(spans[1].column == 3 && spans[1].length == 2,
                 "right terrain must remain solid");
}

bool testRightSpamCrossesTileBoundaries() {
    const std::vector<std::string> grid{std::string(80, '1')};
    const std::vector<TileCollisionSpan> spans = buildHorizontalTileCollisionSpans(grid, isSolid);
    const float endPositionPixels = simulateRightSpam(spans);

    return check(spans.size() == 1, "a flat floor must have no internal vertical seams") &&
           check(endPositionPixels > 300.0f,
                 "right spam must cross many former tile boundaries without becoming stuck");
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
               !TileSemantics::isBreakable('1'),
               "ground tile '1' must not be breakable"
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
                            testCeilingContactTargetsOwningTile();

    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
