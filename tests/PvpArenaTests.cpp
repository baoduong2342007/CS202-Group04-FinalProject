/**
 * @file PvpArenaTests.cpp
 * @author TV5 (Truyen)
 * @brief PvP arena layout validation and Level duel-mode loading tests
 * @note Covers the PVP_ARENA TileMap contract, the shipped arena fixture and
 *       Level::loadPvpArena (fighters, stats, flower lifecycle).
 */

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "core/DisplayConfig.h"
#include "entities/Mario.h"
#include "level/Level.h"
#include "level/TileMap.h"
#include "states/PvpPlayState.h"

namespace {

const std::filesystem::path TEST_DIRECTORY =
    std::filesystem::temp_directory_path() / "super_mario_pvp_arena_tests";

void cleanTestDirectory() {
    std::error_code errorCode;

    std::filesystem::remove_all(TEST_DIRECTORY, errorCode);
    errorCode.clear();
    std::filesystem::create_directories(TEST_DIRECTORY, errorCode);
    assert(!errorCode);
}

std::filesystem::path writeArenaFile(const std::string& fileName,
                                     const std::vector<std::string>& rows) {
    const std::filesystem::path path = TEST_DIRECTORY / fileName;

    std::ofstream output(path, std::ios::trunc);
    assert(output.is_open());

    for (const std::string& row : rows) {
        output << row << std::endl;
    }
    assert(output.good());
    output.close();

    return path;
}

std::vector<std::string> makeValidArena() {
    return {
        "........",
        "...W....",
        "...S....",
        "........",
        ".M..m...",
        "00000000"
    };
}

bool acceptsPvpArena(const std::string& fileName,
                     const std::vector<std::string>& rows) {
    TileMap tileMap;
    const std::filesystem::path path = writeArenaFile(fileName, rows);
    return tileMap.loadFromFile(path.string(), TileMap::LayoutMode::PVP_ARENA);
}

bool testShippedArenaFixtureLoads() {
    std::cout << "[RUNNING] testShippedArenaFixtureLoads..." << std::endl;

    TileMap tileMap;
    assert(tileMap.loadFromFile("levels/pvp_arena.txt",
                                TileMap::LayoutMode::PVP_ARENA));
    assert(tileMap.getWidth() == 20);
    assert(tileMap.getHeight() == 12);

    assert(tileMap.findTiles('M').size() == 1);
    assert(tileMap.findTiles('m').size() == 1);
    assert(tileMap.findTiles('W').size() == 1);

    // Full ground floor.
    for (int column = 0; column < 20; ++column) {
        assert(tileMap.isSolid(column, 9));
        assert(tileMap.isSolid(column, 10));
        assert(tileMap.isSolid(column, 11));
    }

    // Center pedestal platform with the flower tile directly above it.
    const sf::Vector2i pedestal = tileMap.findTiles('W').front();
    assert(pedestal.x >= 8 && pedestal.x <= 11);
    assert(tileMap.isSolid(pedestal.x, pedestal.y + 1));
    for (int column = 8; column <= 11; ++column) {
        assert(tileMap.isSolid(column, pedestal.y + 1));
    }

    // Open duel floor: the staircases are gone. Two symmetric floating
    // step blocks flank the raised pedestal as the only climb aids; the
    // whole fight band between the walls is otherwise clear air.
    assert(tileMap.isSolid(7, 7) && tileMap.isSolid(12, 7));
    for (int column = 1; column <= 18; ++column) {
        for (int row = 6; row <= 8; ++row) {
            if ((column == 7 || column == 12) && row == 7) {
                continue;
            }
            assert(!tileMap.isSolid(column, row));
        }
    }

    // Both spawns stand on the ground row, player one left of player two.
    const sf::Vector2i p1 = tileMap.findTiles('M').front();
    const sf::Vector2i p2 = tileMap.findTiles('m').front();
    assert(p1.y == p2.y);
    assert(tileMap.isSolid(p1.x, p1.y + 1));
    assert(tileMap.isSolid(p2.x, p2.y + 1));
    assert(p1.x < p2.x);

    std::cout << "[PASSED] testShippedArenaFixtureLoads" << std::endl;
    return true;
}

bool testCampaignModeStillValidates() {
    std::cout << "[RUNNING] testCampaignModeStillValidates..." << std::endl;

    TileMap campaign;
    assert(campaign.loadFromFile("levels/level1.txt"));

    // The arena must be rejected as a campaign layout: it has no finish
    // flagpole and it carries PvP-only markers.
    TileMap arenaAsCampaign;
    assert(!arenaAsCampaign.loadFromFile("levels/pvp_arena.txt"));

    std::cout << "[PASSED] testCampaignModeStillValidates" << std::endl;
    return true;
}

bool testPvpMarkerValidation() {
    std::cout << "[RUNNING] testPvpMarkerValidation..." << std::endl;

    assert(acceptsPvpArena("valid_arena.txt", makeValidArena()));

    auto rows = makeValidArena();
    rows[4][1] = '.'; // player one spawn removed
    assert(!acceptsPvpArena("missing_m.txt", rows));

    rows = makeValidArena();
    rows[4][4] = '.'; // player two spawn removed
    assert(!acceptsPvpArena("missing_second.txt", rows));

    rows = makeValidArena();
    rows[4][3] = 'M'; // second player one spawn
    assert(!acceptsPvpArena("double_m.txt", rows));

    rows = makeValidArena();
    rows[1][3] = '.'; // flower pedestal removed
    assert(!acceptsPvpArena("missing_pedestal.txt", rows));

    rows = makeValidArena();
    rows[3][6] = 'W'; // second pedestal
    assert(!acceptsPvpArena("double_pedestal.txt", rows));

    rows = makeValidArena();
    rows[3][0] = 'F'; // finish markers are forbidden in arenas
    assert(!acceptsPvpArena("finish_in_arena.txt", rows));

    // The same minimal arena is rejected by the campaign rules (no flagpole
    // and PvP-only symbols present).
    TileMap asCampaign;
    const std::filesystem::path path = writeArenaFile("valid_arena.txt",
                                                      makeValidArena());
    assert(!asCampaign.loadFromFile(path.string()));

    std::cout << "[PASSED] testPvpMarkerValidation" << std::endl;
    return true;
}

bool testLevelLoadPvpArena() {
    std::cout << "[RUNNING] testLevelLoadPvpArena..." << std::endl;

    Level level;
    level.setTheme(LevelTheme::OVERWORLD);
    assert(level.loadPvpArena("levels/pvp_arena.txt",
                              CharacterType::MARIO, CharacterType::LUIGI));
    assert(level.isPvpMode());

    Mario* p1 = level.getMario();
    Mario* p2 = level.getMario2();
    assert(p1 && p2);
    assert(p1->getBody() && p2->getBody());

    // Identity and authentic campaign stats per fighter.
    assert(p1->getCharacterType() == CharacterType::MARIO);
    assert(p2->getCharacterType() == CharacterType::LUIGI);
    assert(p1->getJumpForce() == 460.f);
    assert(p1->getCharacterProfile().runMaxSpeed == 280.f);
    assert(p2->getJumpForce() == 510.f);
    assert(p2->getCharacterProfile().runMaxSpeed == 250.f);

    // Distinct spawn sides.
    assert(p1->getPosition().x < p2->getPosition().x);

    // Mirror picks are allowed: both fighters may share an identity.
    Level mirror;
    mirror.setTheme(LevelTheme::OVERWORLD);
    assert(mirror.loadPvpArena("levels/pvp_arena.txt",
                               CharacterType::LUIGI, CharacterType::LUIGI));
    assert(mirror.getMario()->getCharacterType() == CharacterType::LUIGI);
    assert(mirror.getMario2()->getCharacterType() == CharacterType::LUIGI);

    std::cout << "[PASSED] testLevelLoadPvpArena" << std::endl;
    return true;
}

bool testFireFlowerLifecycle() {
    std::cout << "[RUNNING] testFireFlowerLifecycle..." << std::endl;

    Level level;
    level.setTheme(LevelTheme::OVERWORLD);
    assert(level.loadPvpArena("levels/pvp_arena.txt",
                              CharacterType::MARIO, CharacterType::LUIGI));

    const sf::Vector2f pedestal(9 * 32.f, 3 * 32.f);
    assert(!level.hasPvpFireFlowerOnField());

    // Only one flower may exist on the field at a time.
    FireFlower* flower = level.spawnPvpFireFlower(pedestal);
    assert(flower != nullptr);
    assert(level.hasPvpFireFlowerOnField());
    assert(level.spawnPvpFireFlower(pedestal) == nullptr);

    // Clearing removes it; a new one can then spawn.
    level.clearPvpFireFlower();
    level.update(1.f / 60.f);
    assert(!level.hasPvpFireFlowerOnField());
    assert(level.spawnPvpFireFlower(pedestal) != nullptr);

    std::cout << "[PASSED] testFireFlowerLifecycle" << std::endl;
    return true;
}

bool testPvpCameraVerticalElevation() {
    std::cout << "[RUNNING] testPvpCameraVerticalElevation..." << std::endl;

    Level level;
    level.setTheme(LevelTheme::OVERWORLD);
    level.setCameraVerticalMode(CameraVerticalMode::DEAD_ZONE);
    assert(level.loadPvpArena("levels/pvp_arena.txt",
                              CharacterType::MARIO, CharacterType::LUIGI));

    assert(level.getCamera().getVerticalMode() == CameraVerticalMode::DEAD_ZONE);

    // Initial camera center rests at the bottom of the arena
    level.update(0.016f);
    const float restingY = level.getCamera().getView().getCenter().y;

    // Simulate Player Two jumping high (Y near top of arena, e.g. 30.f)
    if (Mario* p2 = level.getMario2()) {
        p2->setPosition({320.f, 30.f});
    }

    level.update(0.016f);
    const float elevatedY = level.getCamera().getView().getCenter().y;
    assert(elevatedY < restingY); // Camera elevated upwards!

    // When Player Two descends back to the ground, camera returns to resting position
    if (Mario* p2 = level.getMario2()) {
        p2->setPosition({320.f, 250.f});
    }
    level.update(0.016f);
    const float returnedY = level.getCamera().getView().getCenter().y;
    assert(std::abs(returnedY - restingY) < 1.0f);

    std::cout << "[PASSED] testPvpCameraVerticalElevation" << std::endl;
    return true;
}

bool testPvpCeilingClampAndContainment() {
    std::cout << "[RUNNING] testPvpCeilingClampAndContainment..." << std::endl;

    Level level;
    level.setTheme(LevelTheme::OVERWORLD);
    level.setCameraVerticalMode(CameraVerticalMode::DEAD_ZONE);
    assert(level.loadPvpArena("levels/pvp_arena.txt",
                              CharacterType::MARIO, CharacterType::LUIGI));

    Mario* p1 = level.getMario();
    Mario* p2 = level.getMario2();
    assert(p1 && p2);
    assert(p1->isCeilingClampEnabled());
    assert(p2->isCeilingClampEnabled());

    // 1. Simulate Player Two launching upward with high jump velocity from pedestal (row 5)
    p2->setPosition({320.f, 160.f});
    p2->setVelocity({0.f, -800.f}); // Extreme upward impulse

    // Step physics & PvP update
    for (int frame = 0; frame < 30; ++frame) {
        level.update(1.f / 60.f);
        // Player Two top edge (Y position) must NEVER cross above Y = 0 (top of screen / arena)
        assert(p2->getPosition().y >= 0.0f);
    }

    // 2. Test horizontal screen containment
    p1->setPosition({-50.f, 200.f});
    p1->setVelocity({-300.f, 0.f});
    level.update(1.f / 60.f);
    // Player One clamped inside left boundary
    assert(p1->getPosition().x >= 0.0f);

    p1->setPosition({700.f, 200.f});
    p1->setVelocity({300.f, 0.f});
    level.update(1.f / 60.f);
    // Player One clamped inside right boundary
    assert(p1->getPosition().x <= 640.f);

    std::cout << "[PASSED] testPvpCeilingClampAndContainment" << std::endl;
    return true;
}

bool testPvpPlayStateHudRenderSnapshot() {
    std::cout << "[RUNNING] testPvpPlayStateHudRenderSnapshot..." << std::endl;

    PvpPlayState state(CharacterType::MARIO, CharacterType::LUIGI);
    state.onEnter();

    // Advance 10 frames to settle intro/HUD
    for (int i = 0; i < 10; ++i) {
        state.update(1.f / 60.f);
    }

    sf::RenderTexture rt({DisplayConfig::LOGICAL_WIDTH, DisplayConfig::LOGICAL_HEIGHT});
    rt.clear(sf::Color::Black);
    state.render(rt);
    rt.display();

    // Fast forward into FIGHT phase and give P1 a fire flower
    for (int i = 0; i < 150; ++i) {
        state.update(1.f / 60.f);
    }
    state.setNextFlowerCountdown(0.f);
    state.update(1.f / 60.f);

    Level* level = state.getLevel();
    if (level && level->getMario()) {
        const auto pedestal = level->getTileMap().findTiles('W');
        if (!pedestal.empty()) {
            level->getMario()->setPosition(TileMap::gridToWorldPosition(pedestal.front()));
            for (int i = 0; i < 5; ++i) {
                state.update(1.f / 60.f);
            }
        }
    }

    rt.clear(sf::Color::Black);
    state.render(rt);
    rt.display();

    std::cout << "[PASSED] testPvpPlayStateHudRenderSnapshot" << std::endl;
    return true;
}

} // namespace

int main() {
    cleanTestDirectory();

    const bool success = testShippedArenaFixtureLoads() &&
                         testCampaignModeStillValidates() &&
                         testPvpMarkerValidation() &&
                         testLevelLoadPvpArena() &&
                         testFireFlowerLifecycle() &&
                         testPvpCameraVerticalElevation() &&
                         testPvpCeilingClampAndContainment() &&
                         testPvpPlayStateHudRenderSnapshot();

    std::error_code errorCode;
    std::filesystem::remove_all(TEST_DIRECTORY, errorCode);

    if (success) {
        std::cout << "All PvpArena tests passed successfully!" << std::endl;
        return 0;
    }

    std::cout << "PvpArena tests FAILED." << std::endl;
    return 1;
}
