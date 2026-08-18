/**
 * @file LevelCatalogTests.cpp
 * @author TV1 (Dương)
 * @brief Headless regression tests for the Sprint 6 level-catalog and
 *        GameProgress contracts (S6-TV1-05 / 06 / 07 / 08).
 *
 * NOTE: This is a NEW test file. It intentionally does NOT modify any of the
 * existing test files (which contain pre-existing asserts). It only needs the
 * core headers (LevelCatalog.h, GameProgress.h) — no SFML window/audio context.
 */

#include <cassert>
#include <filesystem>
#include <iostream>
#include <string>

#include "core/LevelCatalog.h"
#include "core/GameProgress.h"
#include "level/Level.h"

namespace {

bool testCatalogHasExactlyFourReleaseLevels() {
    std::cout << "[RUNNING] testCatalogHasExactlyFourReleaseLevels..." << std::endl;

    const auto& catalog = LevelCatalog::getAll();
    assert(catalog.size() == 4);
    assert(LevelCatalog::count() == 4);
    assert(catalog[0].number == 1);
    assert(catalog[1].number == 2);
    assert(catalog[2].number == 3);
    assert(catalog[3].number == 4);

    // World labels per locked Sprint 7 plan: 1-1, 1-2, 1-3, 1-4.
    assert(catalog[0].worldLabel == "1-1");
    assert(catalog[0].filePath == "levels/level1.txt");
    assert(catalog[0].theme == LevelTheme::OVERWORLD);
    assert(catalog[0].music == MusicId::OVERWORLD);
    assert(catalog[0].cameraMode == CameraVerticalMode::DEAD_ZONE);

    assert(catalog[1].worldLabel == "1-2");
    assert(catalog[1].filePath == "levels/level2.txt");
    assert(catalog[1].theme == LevelTheme::UNDERGROUND);
    assert(catalog[1].music == MusicId::UNDERGROUND);
    assert(catalog[1].cameraMode == CameraVerticalMode::DEAD_ZONE);

    assert(catalog[2].worldLabel == "1-3");
    assert(catalog[2].filePath == "levels/level3.txt");
    assert(catalog[2].theme == LevelTheme::OVERWORLD);
    assert(catalog[2].music == MusicId::OVERWORLD);
    assert(catalog[2].cameraMode == CameraVerticalMode::DEAD_ZONE);

    assert(catalog[3].worldLabel == "1-4");
    assert(catalog[3].filePath == "levels/level4.txt");
    assert(catalog[3].theme == LevelTheme::CASTLE);
    assert(catalog[3].music == MusicId::CASTLE);
    assert(catalog[3].cameraMode == CameraVerticalMode::DEAD_ZONE);

    // Every configured ID resolves to its corresponding catalog entry.
    assert(LevelCatalog::find(1) == &catalog[0]);
    assert(LevelCatalog::find(2) == &catalog[1]);
    assert(LevelCatalog::find(3) == &catalog[2]);
    assert(LevelCatalog::find(4) == &catalog[3]);
    // IDs outside the one-based release range are not exposed.
    assert(LevelCatalog::find(0) == nullptr);
    assert(LevelCatalog::find(5) == nullptr);
    // All four release levels let the camera follow Mario vertically.
    for (const LevelDefinition& def : catalog) {
        assert(def.cameraMode == CameraVerticalMode::DEAD_ZONE);
    }
    std::cout << "[PASSED] testCatalogHasExactlyFourReleaseLevels" << std::endl;
    return true;
}

bool testCatalogDoesNotExposeLevelZero() {
    std::cout << "[RUNNING] testCatalogDoesNotExposeLevelZero..." << std::endl;

    // S6-TV1-06: Level 0 (test fixture) must not appear in the release catalog.
    assert(LevelCatalog::find(0) == nullptr);

    // New Game starts at Level 1.
    GameProgress fresh;
    assert(fresh.currentLevel == 1);
    assert(fresh.character == CharacterType::MARIO);

    std::cout << "[PASSED] testCatalogDoesNotExposeLevelZero" << std::endl;
    return true;
}

bool testFinalLevelBoundary() {
    std::cout << "[RUNNING] testFinalLevelBoundary..." << std::endl;

    // Completing Level 4 increments to 5, which is the Win boundary.
    assert(!LevelCatalog::isPastFinalLevel(1));
    assert(!LevelCatalog::isPastFinalLevel(2));
    assert(!LevelCatalog::isPastFinalLevel(3));
    assert(!LevelCatalog::isPastFinalLevel(4));
    assert(LevelCatalog::isPastFinalLevel(5));

    std::cout << "[PASSED] testFinalLevelBoundary" << std::endl;
    return true;
}

bool testGameProgressDefaultsAndMembership() {
    std::cout << "[RUNNING] testGameProgressDefaultsAndMembership..." << std::endl;

    // S6-TV1-08: session progress independent of Level lifetime.
    GameProgress p;
    assert(p.score == 0);
    assert(p.coins == 0);
    assert(p.lives == 3);
    assert(p.power == MarioState::SMALL);
    assert(p.character == CharacterType::MARIO);
    assert(p.currentLevel == 1);

    // It is a value type: modifying one instance must not affect another,
    // reinforcing that destroying a Level cannot reset session progress.
    GameProgress q = p;
    q.score = 500;
    q.lives = 1;
    q.character = CharacterType::LUIGI;
    assert(q.score == 500 && q.lives == 1);
    assert(q.character == CharacterType::LUIGI);
    assert(p.score == 0 && p.lives == 3);
    assert(p.character == CharacterType::MARIO);

    std::cout << "[PASSED] testGameProgressDefaultsAndMembership" << std::endl;
    return true;
}

bool testLevelSpawnHonorsCharacterIdentity() {
    std::cout << "[RUNNING] testLevelSpawnHonorsCharacterIdentity..." << std::endl;

    // CTest normally runs from a build directory, while direct invocations
    // often run from the project root. Resolve the root so this remains a
    // deterministic behavior test rather than a working-directory assertion.
    std::filesystem::path projectRoot = std::filesystem::current_path();
    while (!std::filesystem::exists(projectRoot / "levels" / "level1.txt") &&
           projectRoot != projectRoot.root_path()) {
        projectRoot = projectRoot.parent_path();
    }
    assert(std::filesystem::exists(projectRoot / "levels" / "level1.txt"));

    const std::filesystem::path originalDirectory =
        std::filesystem::current_path();
    std::filesystem::current_path(projectRoot);

    Level luigiLevel;
    assert(luigiLevel.loadFromFile("levels/level1.txt", CharacterType::LUIGI));
    assert(luigiLevel.getCharacterType() == CharacterType::LUIGI);
    assert(luigiLevel.getMario() != nullptr);
    assert(luigiLevel.getMario()->getCharacterType() == CharacterType::LUIGI);

    Level defaultLevel;
    assert(defaultLevel.loadFromFile("levels/level1.txt"));
    assert(defaultLevel.getCharacterType() == CharacterType::MARIO);
    assert(defaultLevel.getMario() != nullptr);
    assert(defaultLevel.getMario()->getCharacterType() == CharacterType::MARIO);

    std::filesystem::current_path(originalDirectory);
    std::cout << "[PASSED] testLevelSpawnHonorsCharacterIdentity" << std::endl;
    return true;
}

} // namespace

int main() {
    const bool ok = testCatalogHasExactlyFourReleaseLevels()
                 && testCatalogDoesNotExposeLevelZero()
                 && testFinalLevelBoundary()
                 && testGameProgressDefaultsAndMembership()
                 && testLevelSpawnHonorsCharacterIdentity();

    if (ok) {
        std::cout << "All LevelCatalogTests passed successfully!" << std::endl;
        return 0;
    }
    std::cout << "LevelCatalogTests FAILED." << std::endl;
    return 1;
}
