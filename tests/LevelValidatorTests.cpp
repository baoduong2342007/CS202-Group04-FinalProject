/**
 * @file LevelValidatorTests.cpp
 * @author TV4 (Vy)
 * @brief Regression tests for level-file validation and the Level 0 fixture
 * @note Sprint 6 - covers S6-TV4-01 and S6-TV4-20
 */

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "level/TileMap.h"

namespace {

const std::filesystem::path TEST_DIRECTORY = std::filesystem::temp_directory_path() / "super_mario_level_validator_tests";

void cleanTestDirectory() {
    std::error_code errorCode;

    std::filesystem::remove_all(TEST_DIRECTORY, errorCode);

    errorCode.clear();

    std::filesystem::create_directories(TEST_DIRECTORY, errorCode);

    assert(!errorCode);
}

std::filesystem::path writeLevelFile(const std::string& fileName, const std::vector<std::string>& rows) {
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

std::vector<std::string> makeValidLevel() {
    return {
        "........",
        ".M....F.",
        "......|.",
        "......|.",
        "11111111"
    };
}

bool rejectsLevel(const std::string& fileName, const std::vector<std::string>& rows) {
    TileMap tileMap;

    const std::filesystem::path path = writeLevelFile(fileName, rows);

    return !tileMap.loadFromFile(path.string());
}

bool testLevelZeroFixtureLoads() {
    std::cout << "[RUNNING] testLevelZeroFixtureLoads..." << std::endl;

    TileMap tileMap;

    assert(tileMap.loadFromFile("levels/level0.txt"));

    assert(tileMap.getWidth() == 100);
    assert(tileMap.getHeight() == 12);

    assert(tileMap.findTiles('M').size() == 1);
    assert(tileMap.findTiles('F').size() == 1);

    std::cout << "[PASSED] testLevelZeroFixtureLoads" << std::endl;

    return true;
}

bool testRejectsInconsistentWidth() {
    std::cout << "[RUNNING] testRejectsInconsistentWidth..." << std::endl;

    auto rows = makeValidLevel();

    rows[0].pop_back();

    assert(rejectsLevel("invalid_width.txt", rows));

    std::cout << "[PASSED] testRejectsInconsistentWidth" << std::endl;

    return true;
}

bool testRejectsInvalidSymbol() {
    std::cout << "[RUNNING] testRejectsInvalidSymbol..." << std::endl;

    auto rows = makeValidLevel();

    rows[0][0] = 'X';

    assert(rejectsLevel("invalid_symbol.txt", rows));

    std::cout << "[PASSED] testRejectsInvalidSymbol" << std::endl;

    return true;
}

bool testRejectsMissingMarioSpawn() {
    std::cout << "[RUNNING] testRejectsMissingMarioSpawn..." << std::endl;

    auto rows = makeValidLevel();

    rows[1][1] = '.';

    assert(rejectsLevel("missing_mario.txt", rows));

    std::cout << "[PASSED] testRejectsMissingMarioSpawn" << std::endl;

    return true;
}

bool testRejectsDuplicateMarioSpawn() {
    std::cout << "[RUNNING] testRejectsDuplicateMarioSpawn..." << std::endl;

    auto rows = makeValidLevel();

    rows[0][0] = 'M';

    assert(rejectsLevel("duplicate_mario.txt", rows));

    std::cout << "[PASSED] testRejectsDuplicateMarioSpawn" << std::endl;

    return true;
}

bool testRejectsMissingFinish() {
    std::cout << "[RUNNING] testRejectsMissingFinish..." << std::endl;

    auto rows = makeValidLevel();

    rows[1][6] = '.';

    assert(rejectsLevel("missing_finish.txt", rows));

    std::cout << "[PASSED] testRejectsMissingFinish" << std::endl;

    return true;
}

bool testRejectsDuplicateFinish() {
    std::cout << "[RUNNING] testRejectsDuplicateFinish..." << std::endl;

    auto rows = makeValidLevel();

    rows[0][0] = 'F';

    assert(rejectsLevel("duplicate_finish.txt", rows));

    std::cout << "[PASSED] testRejectsDuplicateFinish" << std::endl;

    return true;
}

bool testRejectsInvalidFlagPole() {
    std::cout << "[RUNNING] testRejectsInvalidFlagPole..." << std::endl;

    auto rows = makeValidLevel();

    rows[2][6] = '.';

    assert(rejectsLevel("invalid_flag_pole.txt", rows));

    std::cout << "[PASSED] testRejectsInvalidFlagPole" << std::endl;

    return true;
}

} // namespace

int main() {
    cleanTestDirectory();

    const bool success = testLevelZeroFixtureLoads() &&
                         testRejectsInconsistentWidth() &&
                         testRejectsInvalidSymbol() &&
                         testRejectsMissingMarioSpawn() &&
                         testRejectsDuplicateMarioSpawn() &&
                         testRejectsMissingFinish() &&
                         testRejectsDuplicateFinish() &&
                         testRejectsInvalidFlagPole();

    std::error_code errorCode;

    std::filesystem::remove_all(TEST_DIRECTORY, errorCode);

    if (success) {
        std::cout << "All LevelValidator tests passed successfully!" << std::endl;

        return 0;
    }

    std::cout << "LevelValidator tests FAILED." << std::endl;

    return 1;
}
