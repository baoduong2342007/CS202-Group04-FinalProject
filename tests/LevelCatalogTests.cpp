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
#include <iostream>
#include <string>

#include "core/LevelCatalog.h"
#include "core/GameProgress.h"

namespace {

bool testCatalogHasExactlyFourReleaseLevels() {
    std::cout << "[RUNNING] testCatalogHasExactlyFourReleaseLevels..." << std::endl;

    const auto& catalog = LevelCatalog::getAll();
    // S6-TV1-05: exactly four release levels (1, 2, 3, 4), one-based.
    assert(catalog.size() == 4);
    assert(catalog[0].number == 1);
    assert(catalog[1].number == 2);
    assert(catalog[2].number == 3);
    assert(catalog[3].number == 4);

    // World labels per plan: 1-1, 1-2, 1-3, 1-4.
    assert(catalog[0].worldLabel == "1-1");
    assert(catalog[1].worldLabel == "1-2");
    assert(catalog[2].worldLabel == "1-3");
    assert(catalog[3].worldLabel == "1-4");

    assert(catalog[3].theme == LevelTheme::CASTLE);
    assert(catalog[3].music == MusicId::CASTLE);
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

    std::cout << "[PASSED] testCatalogDoesNotExposeLevelZero" << std::endl;
    return true;
}

bool testFinalLevelBoundary() {
    std::cout << "[RUNNING] testFinalLevelBoundary..." << std::endl;

    // S6-TV1-14: completing Level 4 (-> level number 5) means past final level -> Win.
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
    assert(p.currentLevel == 1);

    // It is a value type: modifying one instance must not affect another,
    // reinforcing that destroying a Level cannot reset session progress.
    GameProgress q = p;
    q.score = 500;
    q.lives = 1;
    assert(q.score == 500 && q.lives == 1);
    assert(p.score == 0 && p.lives == 3);

    std::cout << "[PASSED] testGameProgressDefaultsAndMembership" << std::endl;
    return true;
}

} // namespace

int main() {
    const bool ok = testCatalogHasExactlyFourReleaseLevels()
                 && testCatalogDoesNotExposeLevelZero()
                 && testFinalLevelBoundary()
                 && testGameProgressDefaultsAndMembership();

    if (ok) {
        std::cout << "All LevelCatalogTests passed successfully!" << std::endl;
        return 0;
    }
    std::cout << "LevelCatalogTests FAILED." << std::endl;
    return 1;
}
