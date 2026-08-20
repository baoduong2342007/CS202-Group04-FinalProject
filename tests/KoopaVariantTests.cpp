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

    // 2. Test behavioral ledge probing on Question Blocks vs Air for Goomba, Koopa, Spiny, HammerBro
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

    // Koopa (32x48): tile at Y = 64 (row 2) aligns foot to row 3 (Y = 96).
    // Facing RIGHT at col 1 (x = 32): front probe lands in col 2 ('?'), which is enemy support -> does not reverse.
    Koopa koopa1({32.f, 64.f}, &world, LevelTheme::OVERWORLD);
    koopa1.setTileMap(&ledgeMap);
    koopa1.setFacingDirection(Direction::RIGHT);
    advance(koopa1, 0.1f);
    assert(koopa1.getFacingDirection() == Direction::RIGHT);

    // Koopa facing RIGHT at col 2 (x = 64): front probe lands in col 3 ('.'), which is air -> reverses to LEFT.
    Koopa koopa2({64.f, 64.f}, &world, LevelTheme::OVERWORLD);
    koopa2.setTileMap(&ledgeMap);
    koopa2.setFacingDirection(Direction::RIGHT);
    advance(koopa2, 0.1f);
    assert(koopa2.getFacingDirection() == Direction::LEFT);

    // Goomba (32x32): foot on row 3 -> position.y = 96 - 32 = 64.
    Goomba goomba1({32.f, 64.f}, &world, LevelTheme::OVERWORLD);
    goomba1.setTileMap(&ledgeMap);
    goomba1.setFacingDirection(Direction::RIGHT);
    advance(goomba1, 0.1f);
    assert(goomba1.getFacingDirection() == Direction::RIGHT);

    Goomba goomba2({64.f, 64.f}, &world, LevelTheme::OVERWORLD);
    goomba2.setTileMap(&ledgeMap);
    goomba2.setFacingDirection(Direction::RIGHT);
    advance(goomba2, 0.1f);
    assert(goomba2.getFacingDirection() == Direction::LEFT);

    // Spiny (32x32): foot on row 3 -> position.y = 96 - 32 = 64.
    Spiny spiny1({32.f, 64.f}, &world, LevelTheme::OVERWORLD, Direction::RIGHT);
    spiny1.setTileMap(&ledgeMap);
    advance(spiny1, 0.1f);
    assert(spiny1.getFacingDirection() == Direction::RIGHT);

    Spiny spiny2({64.f, 64.f}, &world, LevelTheme::OVERWORLD, Direction::RIGHT);
    spiny2.setTileMap(&ledgeMap);
    advance(spiny2, 0.1f);
    assert(spiny2.getFacingDirection() == Direction::LEFT);

    // HammerBro (32x48): foot on row 3 -> position.y = 96 - 48 = 48.
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

    // Clean up temporary directory
    std::filesystem::remove_all(tempDir, ec);

    std::cout << "[PASSED] testEnemySupportTileCoverageAndLedgeProbing" << std::endl;
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
    std::cout << "All Koopa Variant tests PASSED successfully!" << std::endl;
    return 0;
}
