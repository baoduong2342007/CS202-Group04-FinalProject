/**
 * @file KoopaVariantTests.cpp
 * @brief Unit tests for the Koopa shell wake-up cycle and the Koopa variants
 *        (Red Koopa palette variant, Paratroopa hop/fly modes + wing clip)
 */

#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <box2d/box2d.h>

#include "entities/Koopa.h"
#include "entities/RedKoopa.h"
#include "entities/Paratroopa.h"
#include "entities/BuzzyBeetle.h"
#include "entities/Goomba.h"
#include "entities/Spiny.h"
#include "entities/HammerBro.h"
#include "entities/Mario.h"
#include "level/TileMap.h"
#include "patterns/EntityFactory.h"
#include "physics/CollisionManager.h"

namespace {

b2World makeWorld() {
    return b2World(b2Vec2(0.f, 25.f));
}

/// Advance the enemy's update loop for the given simulated seconds.
void advance(Enemy& enemy, float seconds, float dt = 0.1f) {
    for (float t = 0.f; t < seconds; t += dt) {
        enemy.update(dt);
    }
}

} // namespace

void testShellWakeUpCycle() {
    std::cout << "[RUNNING] testShellWakeUpCycle..." << std::endl;

    b2World world = makeWorld();
    Koopa koopa({100.f, 100.f}, &world, LevelTheme::OVERWORLD);

    koopa.onStomp();
    assert(koopa.getState() == KoopaState::SHELL_IDLE);

    // Still a shell just before the wake delay elapses.
    advance(koopa, 4.5f);
    assert(koopa.getState() == KoopaState::SHELL_IDLE);

    // After the delay the Koopa shell shows the legs-out warning.
    advance(koopa, 0.7f);
    assert(koopa.getState() == KoopaState::SHELL_WAKING);

    // The warning window (1.5 s) ends with the Koopa walking again.
    advance(koopa, 1.6f);
    assert(koopa.getState() == KoopaState::WALKING);
    assert(!koopa.isInShell());

    std::cout << "[PASSED] testShellWakeUpCycle" << std::endl;
}

void testStompAndKickDuringWake() {
    std::cout << "[RUNNING] testStompAndKickDuringWake..." << std::endl;

    b2World world = makeWorld();

    // Stomping a waking shell pushes it back into the full shell.
    Koopa stomped({100.f, 100.f}, &world, LevelTheme::OVERWORLD);
    stomped.onStomp();
    advance(stomped, 5.6f);
    assert(stomped.getState() == KoopaState::SHELL_WAKING);
    stomped.onStomp();
    assert(stomped.getState() == KoopaState::SHELL_IDLE);

    // Kicking is legal while the shell is waking.
    Koopa kicked({200.f, 100.f}, &world, LevelTheme::OVERWORLD);
    kicked.onStomp();
    advance(kicked, 5.6f);
    assert(kicked.getState() == KoopaState::SHELL_WAKING);
    kicked.kick(Direction::RIGHT);
    assert(kicked.getState() == KoopaState::SHELL_SLIDING);
    assert(std::abs(kicked.getVelocity().x - 240.f) < 0.1f);

    std::cout << "[PASSED] testStompAndKickDuringWake" << std::endl;
}

void testRedKoopaVariant() {
    std::cout << "[RUNNING] testRedKoopaVariant..." << std::endl;

    b2World world = makeWorld();
    auto entity = EntityFactory::createFromTileCode('k', {100.f, 100.f}, &world,
                                                    LevelTheme::UNDERGROUND);
    assert(entity != nullptr);
    assert(entity->isEnemy());
    assert(entity->isKoopa());
    assert(entity->isRedKoopa());
    assert(!entity->isParatroopa());

    RedKoopa* red = static_cast<RedKoopa*>(entity.get());
    assert(red->getState() == KoopaState::WALKING);
    assert(red->getSize() == sf::Vector2f(32.f, 48.f));

    // The shell state machine is fully shared with the green Koopa.
    red->onStomp();
    assert(red->isInShell());
    assert(!red->isShellSliding());

    std::cout << "[PASSED] testRedKoopaVariant" << std::endl;
}

void testParatroopaWingClip() {
    std::cout << "[RUNNING] testParatroopaWingClip..." << std::endl;

    b2World world = makeWorld();
    Paratroopa paratroopa({150.f, 100.f}, &world, LevelTheme::OVERWORLD,
                          ParatroopaMode::HOP);
    assert(paratroopa.hasWings());
    assert(paratroopa.isParatroopa());
    assert(paratroopa.isKoopa());

    Mario mario({150.f, 70.f}, {28.f, 30.f});
    mario.initPhysics(&world, b2_dynamicBody, {28.f, 30.f});
    const int initialScore = mario.getScore();

    // First stomp only clips the wings: interaction, not a defeat.
    const bool firstStomp = CollisionManager::defeatEnemy(
        paratroopa, DefeatCause::STOMP, &mario);
    assert(firstStomp);
    assert(!paratroopa.hasWings());
    assert(!paratroopa.isDying());
    assert(paratroopa.getState() == KoopaState::WALKING);
    assert(mario.getScore() > initialScore);

    // Second stomp claims the re-armed latch and starts the shell cycle.
    const bool secondStomp = CollisionManager::defeatEnemy(
        paratroopa, DefeatCause::STOMP, &mario);
    assert(secondStomp);
    assert(paratroopa.getState() == KoopaState::SHELL_IDLE);

    std::cout << "[PASSED] testParatroopaWingClip" << std::endl;
}

void testParatroopaHopMovement() {
    std::cout << "[RUNNING] testParatroopaHopMovement..." << std::endl;

    b2World world = makeWorld();
    Paratroopa hopper({150.f, 100.f}, &world, LevelTheme::OVERWORLD,
                      ParatroopaMode::HOP);

    // Starting at rest on the ground, the first patrol launches a hop.
    hopper.update(0.1f);
    const sf::Vector2f velocity = hopper.getVelocity();
    assert(std::abs(velocity.y + 260.f) < 1.f);
    assert(std::abs(std::abs(velocity.x) - 30.f) < 1.f);

    std::cout << "[PASSED] testParatroopaHopMovement" << std::endl;
}

void testParatroopaFlyVerticalMovement() {
    std::cout << "[RUNNING] testParatroopaFlyVerticalMovement..." << std::endl;

    b2World world = makeWorld();
    Paratroopa flyer({150.f, 100.f}, &world, LevelTheme::OVERWORLD,
                     ParatroopaMode::FLY_VERTICAL);
    assert(flyer.getMode() == ParatroopaMode::FLY_VERTICAL);

    // The flyer floats: no gravity, initial upward drift.
    assert(flyer.getBody() != nullptr);
    flyer.update(0.1f);
    sf::Vector2f velocity = flyer.getVelocity();
    assert(std::abs(velocity.x) < 0.1f);
    assert(std::abs(velocity.y + 80.f) < 1.f);

    // Pushed past the bottom of its corridor, it turns upward.
    const float anchorY = flyer.getPosition().y;
    flyer.getBody()->SetTransform(
        b2Vec2(flyer.getBody()->GetPosition().x,
               flyer.getBody()->GetPosition().y + 96.f / 32.f),
        0.f);
    (void)anchorY;
    flyer.update(0.1f);
    velocity = flyer.getVelocity();
    assert(velocity.y < 0.f);

    std::cout << "[PASSED] testParatroopaFlyVerticalMovement" << std::endl;
}

void testFactoryTileCodes() {
    std::cout << "[RUNNING] testFactoryTileCodes..." << std::endl;

    b2World world = makeWorld();

    auto hop = EntityFactory::createFromTileCode('y', {100.f, 100.f}, &world,
                                                 LevelTheme::OVERWORLD);
    assert(hop != nullptr);
    assert(hop->isParatroopa());
    assert(static_cast<Paratroopa*>(hop.get())->getMode() == ParatroopaMode::HOP);

    auto fly = EntityFactory::createFromTileCode('d', {100.f, 100.f}, &world,
                                                 LevelTheme::OVERWORLD);
    assert(fly != nullptr);
    assert(fly->isParatroopa());
    assert(static_cast<Paratroopa*>(fly.get())->getMode() == ParatroopaMode::FLY_VERTICAL);

    auto byEnum = EntityFactory::createEnemy(EnemyType::BUZZY_BEETLE, {100.f, 100.f},
                                             &world, LevelTheme::OVERWORLD);
    assert(byEnum != nullptr);
    assert(byEnum->isBuzzyBeetle());

    auto redByEnum = EntityFactory::createEnemy(EnemyType::RED_KOOPA, {100.f, 100.f},
                                                &world, LevelTheme::OVERWORLD);
    assert(redByEnum != nullptr);
    assert(redByEnum->isRedKoopa());

    std::cout << "[PASSED] testFactoryTileCodes" << std::endl;
}

void testSlidingShellStompReArm() {
    std::cout << "[RUNNING] testSlidingShellStompReArm..." << std::endl;

    b2World world = makeWorld();
    Mario mario;
    mario.initPhysics(&world, b2_dynamicBody, {32.f, 32.f});
    Koopa koopa({100.f, 100.f}, &world, LevelTheme::OVERWORLD);

    // Stomp 1: Walking -> Shell Idle
    bool firstStomp = CollisionManager::defeatEnemy(koopa, DefeatCause::STOMP, &mario);
    assert(firstStomp);
    assert(koopa.getState() == KoopaState::SHELL_IDLE);

    // Kick: Shell Idle -> Shell Sliding
    koopa.kick(Direction::RIGHT);
    assert(koopa.getState() == KoopaState::SHELL_SLIDING);

    // Stomp 2: Shell Sliding -> Shell Idle (Stopping the sliding shell)
    bool secondStomp = CollisionManager::defeatEnemy(koopa, DefeatCause::STOMP, &mario);
    assert(secondStomp);
    assert(koopa.getState() == KoopaState::SHELL_IDLE);

    std::cout << "[PASSED] testSlidingShellStompReArm" << std::endl;
}

void testWakingKoopaStompReArm() {
    std::cout << "[RUNNING] testWakingKoopaStompReArm..." << std::endl;

    b2World world = makeWorld();
    Mario mario;
    mario.initPhysics(&world, b2_dynamicBody, {32.f, 32.f});
    Koopa koopa({100.f, 100.f}, &world, LevelTheme::OVERWORLD);

    // Stomp 1: Walking -> Shell Idle
    bool firstStomp = CollisionManager::defeatEnemy(koopa, DefeatCause::STOMP, &mario);
    assert(firstStomp);
    assert(koopa.getState() == KoopaState::SHELL_IDLE);

    // Advance until Koopa wakes up into walking
    advance(koopa, 6.5f);
    assert(koopa.getState() == KoopaState::WALKING);

    // Stomp 2: Walking -> Shell Idle again
    bool secondStomp = CollisionManager::defeatEnemy(koopa, DefeatCause::STOMP, &mario);
    assert(secondStomp);
    assert(koopa.getState() == KoopaState::SHELL_IDLE);

    std::cout << "[PASSED] testWakingKoopaStompReArm" << std::endl;
}

void testEnemySupportTileCoverageAndLedgeProbing() {
    std::cout << "[RUNNING] testEnemySupportTileCoverageAndLedgeProbing..." << std::endl;

    // 1. Validate TileMap::isEnemySupport() for all supported and non-supported tile types
    const std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "test_enemy_support_tiles";
    std::error_code ec;
    std::filesystem::remove_all(tempDir, ec);
    std::filesystem::create_directories(tempDir, ec);
    const std::filesystem::path levelFile = tempDir / "support_level.txt";

    const std::vector<std::string> rows = {
        ".......T",
        ".M.....F",
        ".......|",
        "0B?Ufho0",
        "00000000"
    };
    {
        std::ofstream out(levelFile, std::ios::trunc);
        for (const auto& r : rows) {
            out << r << "\n";
        }
    }

    TileMap tileMap;
    assert(tileMap.loadFromFile(levelFile.string()));

    // Row 3: '0' (col 0), 'B' (col 1), '?' (col 2), 'U' (col 3), 'f' (col 4), 'h' (col 5), 'o' (col 6), '0' (col 7)
    assert(tileMap.isEnemySupport(0, 3)); // '0' standard ground
    assert(tileMap.isEnemySupport(1, 3)); // 'B' breakable brick
    assert(tileMap.isEnemySupport(2, 3)); // '?' question block
    assert(tileMap.isEnemySupport(3, 3)); // 'U' 1-up mushroom block
    assert(tileMap.isEnemySupport(4, 3)); // 'f' fire flower block
    assert(tileMap.isEnemySupport(5, 3)); // 'h' hidden/flower block
    assert(tileMap.isEnemySupport(6, 3)); // 'o' star block
    assert(!tileMap.isEnemySupport(0, 0)); // '.' empty air

    // 2. Canonical SMB1 ledge policy: ground walkers (Goomba, green Koopa,
    //    Spiny, wingless Paratroopa, Buzzy Beetle) do NOT reverse at ledges -
    //    they walk off and fall into pits. Only ledge-aware species (Red
    //    Koopa, Hammer Bro) keep reversing and stay on their platform.
    b2World world = makeWorld();

    // Create a level where col 1 is '?' and col 2 is '?', col 3 is '.' (ledge)
    std::filesystem::path ledgeFile = tempDir / "ledge_level.txt";
    const std::vector<std::string> ledgeRows = {
        ".......T",
        ".M.....F",
        ".......|",
        "0??....0",
        "00000000"
    };
    {
        std::ofstream out(ledgeFile, std::ios::trunc);
        for (const auto& r : ledgeRows) {
            out << r << "\n";
        }
    }
    TileMap ledgeMap;
    assert(ledgeMap.loadFromFile(ledgeFile.string()));

    // Goomba (32x32): foot on row 3 -> position.y = 96 - 32 = 64.
    // Support ahead ('?' in col 2): keeps walking RIGHT.
    Goomba goomba1({32.f, 64.f}, &world, LevelTheme::OVERWORLD);
    goomba1.setTileMap(&ledgeMap);
    goomba1.setFacingDirection(Direction::RIGHT);
    advance(goomba1, 0.1f);
    assert(goomba1.getFacingDirection() == Direction::RIGHT);

    // Ledge ahead (air in col 3): canonical Goomba walks off - no reversal.
    Goomba goomba2({64.f, 64.f}, &world, LevelTheme::OVERWORLD);
    goomba2.setTileMap(&ledgeMap);
    goomba2.setFacingDirection(Direction::RIGHT);
    advance(goomba2, 0.1f);
    assert(goomba2.getFacingDirection() == Direction::RIGHT);

    // Green Koopa (32x48, ground-aligned so the foot sits on row 3): walks off.
    Koopa koopa1({32.f, 64.f}, &world, LevelTheme::OVERWORLD);
    koopa1.setTileMap(&ledgeMap);
    koopa1.setFacingDirection(Direction::RIGHT);
    advance(koopa1, 0.1f);
    assert(koopa1.getFacingDirection() == Direction::RIGHT);

    Koopa koopa2({64.f, 64.f}, &world, LevelTheme::OVERWORLD);
    koopa2.setTileMap(&ledgeMap);
    koopa2.setFacingDirection(Direction::RIGHT);
    advance(koopa2, 0.1f);
    assert(koopa2.getFacingDirection() == Direction::RIGHT);

    // Spiny (32x32): foot on row 3 -> position.y = 96 - 32 = 64. Walks off.
    Spiny spiny1({32.f, 64.f}, &world, LevelTheme::OVERWORLD, Direction::RIGHT);
    spiny1.setTileMap(&ledgeMap);
    advance(spiny1, 0.1f);
    assert(spiny1.getFacingDirection() == Direction::RIGHT);

    Spiny spiny2({64.f, 64.f}, &world, LevelTheme::OVERWORLD, Direction::RIGHT);
    spiny2.setTileMap(&ledgeMap);
    advance(spiny2, 0.1f);
    assert(spiny2.getFacingDirection() == Direction::RIGHT);

    // Wingless Paratroopa (a stomp clips the wings; the Koopa core takes
    // over): it inherits the walk-off policy.
    Mario wingClipper({96.f, 64.f}, {28.f, 30.f});
    wingClipper.initPhysics(&world, b2_dynamicBody, {28.f, 30.f});
    Paratroopa wingless({64.f, 64.f}, &world, LevelTheme::OVERWORLD,
                        ParatroopaMode::HOP);
    wingless.setTileMap(&ledgeMap);
    const bool wingsClipped = CollisionManager::defeatEnemy(
        wingless, DefeatCause::STOMP, &wingClipper);
    assert(wingsClipped);
    assert(!wingless.hasWings());
    wingless.setFacingDirection(Direction::RIGHT);
    advance(wingless, 0.1f);
    assert(wingless.getFacingDirection() == Direction::RIGHT);

    // Buzzy Beetle (Koopa shell core, 32x32): walks off.
    BuzzyBeetle buzzy1({32.f, 64.f}, &world, LevelTheme::OVERWORLD);
    buzzy1.setTileMap(&ledgeMap);
    buzzy1.setFacingDirection(Direction::RIGHT);
    advance(buzzy1, 0.1f);
    assert(buzzy1.getFacingDirection() == Direction::RIGHT);

    BuzzyBeetle buzzy2({64.f, 64.f}, &world, LevelTheme::OVERWORLD);
    buzzy2.setTileMap(&ledgeMap);
    buzzy2.setFacingDirection(Direction::RIGHT);
    advance(buzzy2, 0.1f);
    assert(buzzy2.getFacingDirection() == Direction::RIGHT);

    // Red Koopa is the canonical SMB1 ledge guard: support ahead keeps it
    // walking, but a ledge reverses it.
    RedKoopa red1({32.f, 64.f}, &world, LevelTheme::OVERWORLD);
    red1.setTileMap(&ledgeMap);
    red1.setFacingDirection(Direction::RIGHT);
    advance(red1, 0.1f);
    assert(red1.getFacingDirection() == Direction::RIGHT);

    RedKoopa red2({64.f, 64.f}, &world, LevelTheme::OVERWORLD);
    red2.setTileMap(&ledgeMap);
    red2.setFacingDirection(Direction::RIGHT);
    advance(red2, 0.1f);
    assert(red2.getFacingDirection() == Direction::LEFT);

    // HammerBro (32x48): foot on row 3 -> position.y = 96 - 48 = 48.
    // It stays on its platform: the ledge reverses it.
    HammerBro bro1({32.f, 48.f}, &world, LevelTheme::OVERWORLD);
    bro1.setTileMap(&ledgeMap);
    bro1.setFacingDirection(Direction::RIGHT);
    advance(bro1, 0.1f);
    assert(bro1.getFacingDirection() == Direction::RIGHT);

    HammerBro bro2({64.f, 48.f}, &world, LevelTheme::OVERWORLD);
    bro2.setTileMap(&ledgeMap);
    bro2.setFacingDirection(Direction::RIGHT);
    advance(bro2, 0.1f);
    assert(bro2.getFacingDirection() == Direction::LEFT);

    // Wall collisions still turn EVERY walker, ledge policy notwithstanding.
    goomba2.onWallCollision();
    assert(goomba2.getFacingDirection() == Direction::LEFT);
    koopa2.onWallCollision();
    assert(koopa2.getFacingDirection() == Direction::LEFT);
    spiny2.onWallCollision();
    assert(spiny2.getFacingDirection() == Direction::LEFT);
    wingless.onWallCollision();
    assert(wingless.getFacingDirection() == Direction::LEFT);
    buzzy2.onWallCollision();
    assert(buzzy2.getFacingDirection() == Direction::LEFT);
    red2.onWallCollision();
    assert(red2.getFacingDirection() == Direction::RIGHT);
    bro2.onWallCollision();
    assert(bro2.getFacingDirection() == Direction::RIGHT);

    // Clean up temporary directory
    std::filesystem::remove_all(tempDir, ec);

    std::cout << "[PASSED] testEnemySupportTileCoverageAndLedgeProbing" << std::endl;
}

void testNarrowPatrolBreakoutAI() {
    std::cout << "[RUNNING] testNarrowPatrolBreakoutAI..." << std::endl;

    std::error_code ec;
    const std::filesystem::path tempDir =
        std::filesystem::temp_directory_path() / "super_mario_narrow_patrol_tests";
    std::filesystem::create_directories(tempDir, ec);

    // Create a level with an isolated 2-tile platform at row 3 (col 2, 3)
    // and empty air on col 0, 1, 4, 5, 6. Col 7 has the finish pole ending on ground.
    // Below row 3 is air (row 4) and ground (row 5).
    std::filesystem::path levelFile = tempDir / "narrow_level.txt";
    const std::vector<std::string> rows = {
        ".......T",
        ".M.....F",
        ".......|",
        "..00...0",
        ".......0",
        "00000000"
    };
    {
        std::ofstream out(levelFile, std::ios::trunc);
        for (const auto& r : rows) {
            out << r << "\n";
        }
    }
    TileMap map;
    assert(map.loadFromFile(levelFile.string()));

    // Canonical SMB1 ledge policy: ground walkers step off platforms, so the
    // narrow-patrol breakout AI is exercised by the ledge-aware walkers
    // (Red Koopa); a canonical Goomba on the same pocket simply walks off.
    // 1. Red Koopa on 2-tile platform with Mario approaching from the LEFT (x = 0)
    {
        b2World world = makeWorld();
        TileMap map;
        assert(map.loadFromFile(levelFile.string()));
        map.createPhysicsBodies(&world);

        RedKoopa redKoopa({90.f, 64.f}, &world, LevelTheme::OVERWORLD);
        redKoopa.setTileMap(&map);
        redKoopa.updatePlayerPosition({0.f, 64.f}); // Mario is to the left

        // Move right until it hits right ledge and turns LEFT
        redKoopa.setFacingDirection(Direction::RIGHT);
        for (int i = 0; i < 100 && redKoopa.getFacingDirection() == Direction::RIGHT; ++i) {
            world.Step(1.f / 60.f, 8, 3);
            redKoopa.update(1.f / 60.f);
        }
        assert(redKoopa.getFacingDirection() == Direction::LEFT);

        // Move left until it hits left ledge and triggers breakout AI facing LEFT
        for (int i = 0; i < 100 && !redKoopa.isEscapingNarrowRange(); ++i) {
            world.Step(1.f / 60.f, 8, 3);
            redKoopa.update(1.f / 60.f);
        }
        assert(redKoopa.isEscapingNarrowRange());
        assert(redKoopa.getFacingDirection() == Direction::LEFT);

        // While escaping, the Red Koopa ignores the left ledge and steps off the platform (drops down).
        for (int i = 0; i < 100 && redKoopa.isEscapingNarrowRange(); ++i) {
            world.Step(1.f / 60.f, 8, 3);
            redKoopa.update(1.f / 60.f);
        }
        // Once it is clear of the narrow platform, isEscapingNarrowRange resets to false
        assert(!redKoopa.isEscapingNarrowRange());

        // Step simulation for gravity to land it on the bottom ground (row 5)
        for (int i = 0; i < 60; ++i) {
            world.Step(1.f / 60.f, 8, 3);
            redKoopa.update(1.f / 60.f);
        }
        assert(redKoopa.getPosition().y > 80.f); // Has fallen down from the 2-tile platform

        map.destroyPhysicsBodies();
    }

    // 2. Red Koopa on 2-tile platform with Mario approaching from the RIGHT (x = 300)
    {
        b2World world = makeWorld();
        TileMap map;
        assert(map.loadFromFile(levelFile.string()));
        map.createPhysicsBodies(&world);

        RedKoopa redKoopa({70.f, 64.f}, &world, LevelTheme::OVERWORLD);
        redKoopa.setTileMap(&map);
        redKoopa.updatePlayerPosition({300.f, 64.f}); // Mario is to the right

        // Move left until it hits left ledge and turns RIGHT
        redKoopa.setFacingDirection(Direction::LEFT);
        for (int i = 0; i < 100 && redKoopa.getFacingDirection() == Direction::LEFT; ++i) {
            world.Step(1.f / 60.f, 8, 3);
            redKoopa.update(1.f / 60.f);
        }
        assert(redKoopa.getFacingDirection() == Direction::RIGHT);

        // Move right until it hits right ledge and triggers escape mode facing RIGHT
        for (int i = 0; i < 100 && !redKoopa.isEscapingNarrowRange(); ++i) {
            world.Step(1.f / 60.f, 8, 3);
            redKoopa.update(1.f / 60.f);
        }
        assert(redKoopa.isEscapingNarrowRange());
        assert(redKoopa.getFacingDirection() == Direction::RIGHT);

        map.destroyPhysicsBodies();
    }

    // 3. Canonical walker sanity: a Goomba on the same 2-tile pocket never
    //    oscillates - it walks straight off the platform without reversing.
    {
        b2World world = makeWorld();
        TileMap map;
        assert(map.loadFromFile(levelFile.string()));
        map.createPhysicsBodies(&world);

        Goomba goomba({90.f, 64.f}, &world, LevelTheme::OVERWORLD);
        goomba.setTileMap(&map);
        goomba.updatePlayerPosition({0.f, 64.f});

        goomba.setFacingDirection(Direction::RIGHT);
        for (int i = 0; i < 100; ++i) {
            world.Step(1.f / 60.f, 8, 3);
            goomba.update(1.f / 60.f);
        }
        // No ledge reversal, no breakout AI: it kept facing RIGHT, walked off
        // the platform, and fell to the bottom ground (row 5).
        assert(goomba.getFacingDirection() == Direction::RIGHT);
        assert(!goomba.isEscapingNarrowRange());
        assert(goomba.getPosition().y > 80.f); // Has fallen down from the 2-tile platform

        map.destroyPhysicsBodies();
    }

    // 4. Red Koopa on wide platform (5 tiles) - should NOT trigger narrow breakout
    {
        b2World world = makeWorld();
        std::filesystem::path wideFile = tempDir / "wide_level.txt";
        const std::vector<std::string> wideRows = {
            ".......T",
            ".M.....F",
            ".......|",
            ".00000.0",
            ".......0",
            "00000000"
        };
        {
            std::ofstream out(wideFile, std::ios::trunc);
            for (const auto& r : wideRows) {
                out << r << "\n";
            }
        }
        TileMap wideMap;
        assert(wideMap.loadFromFile(wideFile.string()));
        wideMap.createPhysicsBodies(&world);

        RedKoopa wideRed({40.f, 64.f}, &world, LevelTheme::OVERWORLD);
        wideRed.setTileMap(&wideMap);
        wideRed.updatePlayerPosition({0.f, 64.f});

        // Patrol along the 5-tile platform (width = 160px > 80px threshold)
        wideRed.setFacingDirection(Direction::RIGHT);
        for (int i = 0; i < 300; ++i) {
            world.Step(1.f / 60.f, 8, 3);
            wideRed.update(1.f / 60.f);
        }
        // Does not enter escape mode on wide platform
        assert(!wideRed.isEscapingNarrowRange());

        wideMap.destroyPhysicsBodies();
    }

    std::filesystem::remove_all(tempDir, ec);
    std::cout << "[PASSED] testNarrowPatrolBreakoutAI" << std::endl;
}

int main() {
    std::cout << "=== Running Koopa Variant Tests ===" << std::endl;
    testShellWakeUpCycle();
    testStompAndKickDuringWake();
    testRedKoopaVariant();
    testParatroopaWingClip();
    testParatroopaHopMovement();
    testParatroopaFlyVerticalMovement();
    testFactoryTileCodes();
    testSlidingShellStompReArm();
    testWakingKoopaStompReArm();
    testEnemySupportTileCoverageAndLedgeProbing();
    testNarrowPatrolBreakoutAI();
    std::cout << "All Koopa Variant tests PASSED successfully!" << std::endl;
    return 0;
}
