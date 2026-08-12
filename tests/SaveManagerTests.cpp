/**
 * @file SaveManagerTests.cpp
 * @author TV4
 * @brief Regression tests for SaveManager loading, fallback, and monotonic high score persistence
 * @note Sprint 6 — covers S6-TV4-39 save persistence test matrix
 */

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "core/SaveManager.h"
#include "core/LevelCatalog.h"

namespace {

const std::filesystem::path TEST_DIRECTORY = std::filesystem::temp_directory_path() / "super_mario_save_manager_tests";

const std::filesystem::path TEST_SAVE_PATH = TEST_DIRECTORY / "save.txt";

void cleanTestDirectory() {
    std::error_code errorCode;
    std::filesystem::remove_all(TEST_DIRECTORY, errorCode);

    std::filesystem::create_directories(TEST_DIRECTORY, errorCode);

    assert(!errorCode);
}

void writeSaveFile(const std::string& content) {
    std::ofstream output(TEST_SAVE_PATH, std::ios::trunc);

    assert(output.is_open());

    output << content;

    assert(output.good());

    output.close();
}

void assertDefaultData(const SaveData& data) {
    assert(data.version == SAVE_DATA_VERSION);
    assert(data.highScore == 0);
    assert(data.highestUnlockedLevel == 1);
    assert(data.soundVolume == 80.0f);
    assert(data.musicVolume == 70.0f);
}

bool testMissingSaveUsesDefaults() {
    std::cout << "[RUNNING] testMissingSaveUsesDefaults..." << std::endl;

    cleanTestDirectory();

    SaveManager manager(TEST_SAVE_PATH.string());

    assert(!manager.load());
    assertDefaultData(manager.getData());

    std::cout << "[PASSED] testMissingSaveUsesDefaults" << std::endl;

    return true;
}

bool testValidSaveLoadsCorrectly() {
    std::cout << "[RUNNING] testValidSaveLoadsCorrectly..." << std::endl;

    cleanTestDirectory();

    writeSaveFile("version 1\n"
                  "highScore 12500\n"
                  "highestUnlockedLevel 3\n"
                  "soundVolume 65\n"
                  "musicVolume 35\n"
                  );

    SaveManager manager(TEST_SAVE_PATH.string());

    assert(manager.load());

    const SaveData& data = manager.getData();

    assert(data.version == SAVE_DATA_VERSION);
    assert(data.highScore == 12500);
    assert(data.highestUnlockedLevel == 3);
    assert(data.soundVolume == 65.0f);
    assert(data.musicVolume == 35.0f);

    std::cout << "[PASSED] testValidSaveLoadsCorrectly" << std::endl;

    return true;
}

bool testUnlockedLevelIsClampedToReleaseCatalog() {
    std::cout << "[RUNNING] testUnlockedLevelIsClampedToReleaseCatalog..."
              << std::endl;

    cleanTestDirectory();

    writeSaveFile("version 1\n"
                  "highScore 12345\n"
                  "highestUnlockedLevel 4\n"
                  "soundVolume 80\n"
                  "musicVolume 70\n");

    SaveManager manager(TEST_SAVE_PATH.string());

    assert(manager.load());
    assert(manager.getData().highScore == 12345);
    assert(manager.getData().highestUnlockedLevel == LevelCatalog::count());
    assert(!manager.updateHighestUnlockedLevel(99));

    std::cout << "[PASSED] testUnlockedLevelIsClampedToReleaseCatalog"
              << std::endl;

    return true;
}

bool testCorruptSaveUsesDefaults() {
    std::cout << "[RUNNING] testCorruptSaveUsesDefaults..." << std::endl;

    cleanTestDirectory();

    writeSaveFile("version 1\n"
                  "highScore invalid\n"
                  "highestUnlockedLevel 3\n"
                  "soundVolume 65\n"
                  "musicVolume 35\n"
                  );

    SaveManager manager(TEST_SAVE_PATH.string());

    assert(!manager.load());
    assertDefaultData(manager.getData());

    std::cout << "[PASSED] testCorruptSaveUsesDefaults" << std::endl;

    return true;
}

bool testVersionMismatchUsesDefaults() {
    std::cout << "[RUNNING] testVersionMismatchUsesDefaults..." << std::endl;

    cleanTestDirectory();

    writeSaveFile("version 2\n"
                  "highScore 12500\n"
                  "highestUnlockedLevel 3\n"
                  "soundVolume 65\n"
                  "musicVolume 35\n"
                  );

    SaveManager manager(TEST_SAVE_PATH.string());

    assert(!manager.load());
    assertDefaultData(manager.getData());

    std::cout << "[PASSED] testVersionMismatchUsesDefaults" << std::endl;

    return true;
}

bool testHighScoreIsMonotonic() {
    std::cout << "[RUNNING] testHighScoreIsMonotonic..." << std::endl;

    cleanTestDirectory();

    writeSaveFile("version 1\n"
                  "highScore 12500\n"
                  "highestUnlockedLevel 2\n"
                  "soundVolume 80\n"
                  "musicVolume 70\n"
                  );

    SaveManager manager(TEST_SAVE_PATH.string());

    assert(manager.load());

    assert(manager.updateHighScore(18000));
    assert(manager.getData().highScore == 18000);

    assert(!manager.updateHighScore(5000));
    assert(manager.getData().highScore == 18000);

    SaveManager reloadedManager(TEST_SAVE_PATH.string());

    assert(reloadedManager.load());
    assert(reloadedManager.getData().highScore == 18000);

    std::cout << "[PASSED] testHighScoreIsMonotonic" << std::endl;

    return true;
}

bool testSuccessfulSaveRemovesTemporaryFile() {
    std::cout << "[RUNNING] testSuccessfulSaveRemovesTemporaryFile..." << std::endl;

    cleanTestDirectory();

    SaveManager manager(TEST_SAVE_PATH.string());

    assert(manager.updateHighScore(5000));

    const std::filesystem::path temporaryPath = TEST_SAVE_PATH.string() + ".tmp";

    assert(std::filesystem::exists(TEST_SAVE_PATH));
    assert(!std::filesystem::exists(temporaryPath));

    std::cout << "[PASSED] testSuccessfulSaveRemovesTemporaryFile" << std::endl;

    return true;
}

} // namespace

int main() {
    const bool success = testMissingSaveUsesDefaults() &&
                         testValidSaveLoadsCorrectly() &&
                         testUnlockedLevelIsClampedToReleaseCatalog() &&
                         testCorruptSaveUsesDefaults() &&
                         testVersionMismatchUsesDefaults() &&
                         testHighScoreIsMonotonic() &&
                         testSuccessfulSaveRemovesTemporaryFile();

    std::error_code errorCode;
    std::filesystem::remove_all(TEST_DIRECTORY, errorCode);

    if (success) {
        std::cout << "All SaveManager tests passed successfully!" << std::endl;

        return 0;
    }

    std::cout << "SaveManager tests FAILED." << std::endl;

    return 1;
}
