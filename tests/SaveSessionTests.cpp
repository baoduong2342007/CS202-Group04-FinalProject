/**
 * @file SaveSessionTests.cpp
 * @author TV1 (Dương)
 * @brief Session-restart persistence guards for the SaveManager integration
 *        (S6-TV1-19).
 *
 * A "session" is one execution of the game. Restarting the executable creates a
 * brand-new SaveManager on the same save file. This suite simulates that exact
 * restart by constructing two independent SaveManager instances bound to the
 * same temporary file — the first object is destroyed (process exit) before the
 * second one loads it back again.
 *
 * Also guards the S6-TV1-19 composition-root rule: GameManager owns THE single
 * SaveManager and exposes it through getSaveManager(); no state may create a
 * second instance.
 */

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <system_error>

#include "core/GameManager.h"
#include "core/SaveManager.h"
#include "entities/Mario.h"

namespace {

const std::filesystem::path TEST_DIRECTORY =
    std::filesystem::temp_directory_path() / "super_mario_save_session_tests";

const std::filesystem::path TEST_SAVE_PATH = TEST_DIRECTORY / "save.txt";

void cleanTestDirectory() {
    std::error_code errorCode;
    std::filesystem::remove_all(TEST_DIRECTORY, errorCode);

    assert(!errorCode);

    std::filesystem::create_directories(TEST_DIRECTORY, errorCode);

    assert(!errorCode);
}

bool testRestartSessionPersistsAllFields() {
    std::cout << "[RUNNING] testRestartSessionPersistsAllFields..." << std::endl;

    cleanTestDirectory();

    {
        // Session 1 (first run of the game).
        SaveManager firstSession(TEST_SAVE_PATH.string());

        assert(firstSession.updateHighScore(5000));
        assert(firstSession.updateHighestUnlockedLevel(2));
        assert(firstSession.updateAudioSettings(15.f, 25.f));
    } // firstSession destroyed here -> simulates process exit

    // Session 2 (restart of the game): a fresh SaveManager must reload every
    // field persisted by session 1.
    SaveManager restartedSession(TEST_SAVE_PATH.string());

    assert(restartedSession.load());

    const SaveData& data = restartedSession.getData();

    assert(data.version == SAVE_DATA_VERSION);
    assert(data.highScore == 5000);
    assert(data.highestUnlockedLevel == 2);
    assert(data.soundVolume == 15.0f);
    assert(data.musicVolume == 25.0f);

    std::cout << "[PASSED] testRestartSessionPersistsAllFields" << std::endl;
    return true;
}

bool testRestartKeepsMonotonicRules() {
    std::cout << "[RUNNING] testRestartKeepsMonotonicRules..." << std::endl;

    cleanTestDirectory();

    {
        SaveManager firstSession(TEST_SAVE_PATH.string());
        assert(firstSession.updateHighScore(8000));
        assert(firstSession.updateHighestUnlockedLevel(2));
    }

    SaveManager restartedSession(TEST_SAVE_PATH.string());
    assert(restartedSession.load());

    // A lower score must never overwrite the persisted high score after restart.
    assert(!restartedSession.updateHighScore(3000));
    assert(restartedSession.getData().highScore == 8000);

    // An older unlock must never lock previously unlocked levels.
    assert(!restartedSession.updateHighestUnlockedLevel(1));
    assert(restartedSession.getData().highestUnlockedLevel == 2);

    std::cout << "[PASSED] testRestartKeepsMonotonicRules" << std::endl;
    return true;
}

bool testCompositionRootOwnsSingleSaveManager() {
    std::cout << "[RUNNING] testCompositionRootOwnsSingleSaveManager..." << std::endl;

    // S6-TV1-19: every caller (Menu/Win/GameOver/Play/Pause) must share the ONE
    // SaveManager owned by GameManager. Two lookups must address the same object.
    SaveManager& first = GameManager::getInstance().getSaveManager();
    SaveManager& second = GameManager::getInstance().getSaveManager();

    assert(&first == &second);

    // Accessing it also proves the default save data exists (never null/crash).
    (void)first.getData();
    assert(first.getData().version == SAVE_DATA_VERSION);

    std::cout << "[PASSED] testCompositionRootOwnsSingleSaveManager" << std::endl;
    return true;
}

bool testMidSessionDeathSavesHighScore() {
    std::cout << "[RUNNING] testMidSessionDeathSavesHighScore..." << std::endl;

    cleanTestDirectory();

    {
        // Session 1: Mario accumulates 4500 points and dies with lives remaining.
        // PlayState::onNotify(PLAYER_DIED) calls updateHighScore(4500).
        SaveManager dyingSession(TEST_SAVE_PATH.string());
        assert(dyingSession.updateHighScore(4500));
        // Session ends prematurely (e.g. player quits after death, lives remaining > 0).
    }

    // Session 2 (restart execution): new SaveManager loads disk file.
    SaveManager restartedSession(TEST_SAVE_PATH.string());
    assert(restartedSession.load());
    assert(restartedSession.getData().highScore == 4500);

    std::cout << "[PASSED] testMidSessionDeathSavesHighScore" << std::endl;
    return true;
}

} // namespace

int main() {
    const bool ok = testRestartSessionPersistsAllFields()
                 && testRestartKeepsMonotonicRules()
                 && testCompositionRootOwnsSingleSaveManager()
                 && testMidSessionDeathSavesHighScore();

    std::error_code errorCode;
    std::filesystem::remove_all(TEST_DIRECTORY, errorCode);

    if (ok) {
        std::cout << "All SaveSession tests passed successfully!" << std::endl;
        return 0;
    }
    std::cout << "SaveSession tests FAILED." << std::endl;
    return 1;
}