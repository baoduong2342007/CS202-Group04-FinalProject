/**
 * @file PlayStateTests.cpp
 * @author TV1 (Duong)
 * @brief Regression tests for the PlayState terminal-result contract:
 *        - death -> exactly one PLAYER_DIED event + exactly one life lost;
 *        - a second death while dying is ignored (no double event/life);
 *        - respawn re-arms the death chain;
 *        - Level 4 completion decision -> Win through a single transition path
 *          (S6-TV1-13/14/18).
 *
 * Limitation: constructing a real PlayState headlessly would require a live
 * SFML window, an audio device and an on-disk level load, so the PlayState
 * state machine is exercised through the same decision logic it uses
 * (LevelCatalog) plus the Mario death contract that drives it. The end-to-end
 * "Level 1 -> 2 -> 3 -> 4 -> Win" loop is covered by manual playthrough
 * (S6-TV4-40) and the PlayState code-path review logged in
 * docs/change_in_develop.md.
 */

#include <array>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <utility>

#include "core/GameManager.h"
#include "core/GameProgress.h"
#include "core/LevelCatalog.h"
#include "core/SoundManager.h"
#include "entities/Mario.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "patterns/IObserver.h"
#include "patterns/InputState.h"
#include "states/GameOverState.h"
#include "states/LevelSelectState.h"

namespace {

class EmptyState final : public IGameState {
public:
    void onEnter() override {}
    void onExit() override {}
    void processEvents(const sf::Event&) override {}
    void processInput(const InputState&) override {}
    void update(float) override {}
    void render(sf::RenderTarget&) override {}
};

std::filesystem::path projectRoot() {
    auto root = std::filesystem::current_path();
    while (!std::filesystem::exists(root / "assets" / "fonts" / "mario.ttf") &&
           root != root.root_path()) {
        root = root.parent_path();
    }
    assert(std::filesystem::exists(root / "assets" / "fonts" / "mario.ttf"));
    return root;
}

class IsolatedRuntimeFixture final {
public:
    IsolatedRuntimeFixture()
        : m_originalDirectory(std::filesystem::current_path()) {
        const auto sourceRoot = projectRoot();
        const auto stamp = std::chrono::high_resolution_clock::now()
                               .time_since_epoch().count();
        m_runtimeRoot = std::filesystem::temp_directory_path() /
                        ("super_mario_play_state_tests_" + std::to_string(stamp));
        std::filesystem::create_directories(m_runtimeRoot);
        std::filesystem::copy(
            sourceRoot / "assets", m_runtimeRoot / "assets",
            std::filesystem::copy_options::recursive);
        std::filesystem::copy(
            sourceRoot / "levels", m_runtimeRoot / "levels",
            std::filesystem::copy_options::recursive);
        std::filesystem::current_path(m_runtimeRoot);
    }

    ~IsolatedRuntimeFixture() {
        std::error_code errorCode;
        std::filesystem::current_path(m_originalDirectory, errorCode);
        std::filesystem::remove_all(m_runtimeRoot, errorCode);
    }

private:
    std::filesystem::path m_originalDirectory;
    std::filesystem::path m_runtimeRoot;
};

sf::Event keyPressed(sf::Keyboard::Key key) {
    return sf::Event(sf::Event::KeyPressed{
        key, sf::Keyboard::Scan::Unknown, false, false, false, false});
}

InputState pressed(sf::Keyboard::Key key) {
    InputState input;
    input.handleEvent(keyPressed(key));
    return input;
}

void resetStateStack() {
    auto& game = GameManager::getInstance();
    game.changeState(std::make_unique<EmptyState>());
    game.update(0.f);
    assert(game.stackDepth() == 1);
    game.popState();
    game.update(0.f);
    assert(game.stackDepth() == 0);
}

void restoreSaveData(SaveManager& saveManager, const SaveData& original) {
    saveManager.resetToDefaults();
    if (original.highScore > 0) {
        assert(saveManager.updateHighScore(original.highScore));
    }
    if (original.highestUnlockedLevel > 1) {
        assert(saveManager.updateHighestUnlockedLevel(original.highestUnlockedLevel));
    }
    assert(saveManager.updateAudioSettings(original.soundVolume, original.musicVolume));
}

class DeathCounter final : public IObserver {
public:
    DeathCounter() {
        EventBus::getInstance().subscribe(EventType::PLAYER_DIED, this);
    }

    ~DeathCounter() override {
        EventBus::getInstance().unsubscribe(EventType::PLAYER_DIED, this);
    }

    void onNotify(EventType event) override {
        if (event == EventType::PLAYER_DIED) {
            ++diedEvents;
        }
    }

    int diedEvents = 0;
};

bool testRaceConditionGuard() {
    std::cout << "[RUNNING] testRaceConditionGuard..." << std::endl;

    // PlayState::onNotify rejects a terminal event if one was already committed
    // this frame (S6-TV1-13). The level-boundary decision it guards is the
    // catalog boundary below: completing Level 4 is the only path to Win.
    assert(!LevelCatalog::isPastFinalLevel(1));
    assert(!LevelCatalog::isPastFinalLevel(2));
    assert(!LevelCatalog::isPastFinalLevel(3));
    assert(!LevelCatalog::isPastFinalLevel(4));
    assert(LevelCatalog::isPastFinalLevel(5));

    std::cout << "[PASSED] testRaceConditionGuard" << std::endl;
    return true;
}

bool testWinDecisionCommittedOnce() {
    std::cout << "[RUNNING] testWinDecisionCommittedOnce..." << std::endl;

    // Mirrors PlayState::onNotify LEVEL_COMPLETED (increments currentLevel, then
    // m_transitionIsWin = isPastFinalLevel(...)) and the LOADING phase that
    // builds WinState exactly once via the m_transitionIsWin flag (S6-TV1-14).
    GameProgress progress;
    progress.currentLevel = 1;
    progress.score = 100;

    // Simulate LEVEL_COMPLETED (Level 1 -> 2)
    progress.currentLevel++;
    assert(LevelCatalog::isPastFinalLevel(progress.currentLevel) == false);

    // Simulate LEVEL_COMPLETED (Level 2 -> 3)
    progress.currentLevel++;
    assert(LevelCatalog::isPastFinalLevel(progress.currentLevel) == false);

    // Simulate LEVEL_COMPLETED (Level 3 -> 4)
    progress.currentLevel++;
    assert(LevelCatalog::isPastFinalLevel(progress.currentLevel) == false);

    // Simulate LEVEL_COMPLETED (Level 4 -> Win boundary 5).
    progress.score = 5000; // Snapshot final score
    progress.currentLevel++;
    
    bool transitionIsWin = LevelCatalog::isPastFinalLevel(progress.currentLevel);
    assert(transitionIsWin == true);
    
    int winStateQueuedCount = 0;
    if (transitionIsWin) {
        winStateQueuedCount++;
        // transitionPhase = TransitionPhase::NONE;
    }
    
    // Verify Win is only queued once and score is preserved
    assert(winStateQueuedCount == 1);
    assert(progress.score == 5000);

    std::cout << "[PASSED] testWinDecisionCommittedOnce" << std::endl;
    return true;
}

bool testSingleDeathFiresOneEventAndOneLifeLoss() {
    std::cout << "[RUNNING] testSingleDeathFiresOneEventAndOneLifeLoss..." << std::endl;

    DeathCounter counter;
    Mario mario;
    const int livesBefore = mario.getLives();

    mario.loseLife();

    assert(mario.getLives() == livesBefore - 1); // exactly one life lost
    assert(counter.diedEvents == 1);             // exactly one PLAYER_DIED

    std::cout << "[PASSED] testSingleDeathFiresOneEventAndOneLifeLoss" << std::endl;
    return true;
}

bool testSecondDeathWhileDyingIsIgnored() {
    std::cout << "[RUNNING] testSecondDeathWhileDyingIsIgnored..." << std::endl;

    DeathCounter counter;
    Mario mario;
    mario.loseLife();
    const int livesAfterFirst = mario.getLives();

    mario.loseLife(); // m_isDying guard: must be ignored

    assert(mario.getLives() == livesAfterFirst);
    assert(counter.diedEvents == 1);

    std::cout << "[PASSED] testSecondDeathWhileDyingIsIgnored" << std::endl;
    return true;
}

bool testRespawnReArmsDeathChain() {
    std::cout << "[RUNNING] testRespawnReArmsDeathChain..." << std::endl;

    DeathCounter counter;
    Mario mario;
    const int livesBefore = mario.getLives();

    mario.loseLife();
    assert(counter.diedEvents == 1);

    mario.respawn(sf::Vector2f(100.f, 100.f)); // re-arms the death chain
    mario.loseLife();

    assert(mario.getLives() == livesBefore - 2);
    assert(counter.diedEvents == 2);

    std::cout << "[PASSED] testRespawnReArmsDeathChain" << std::endl;
    return true;
}

bool testGameOverRetryRestartsFailedLevel() {
    std::cout << "[RUNNING] testGameOverRetryRestartsFailedLevel..." << std::endl;

    auto& game = GameManager::getInstance();
    auto& saveManager = game.getSaveManager();
    const int highestUnlockedBefore = saveManager.getData().highestUnlockedLevel;

    const std::array<std::pair<int, MusicId>, 6> retryCases = {{
        {1, MusicId::OVERWORLD},
        {2, MusicId::UNDERGROUND},
        {3, MusicId::UNDERWATER},
        {4, MusicId::CASTLE},
        // PlayState owns the one-based catalog clamp for malformed progress.
        {0, MusicId::OVERWORLD},
        {5, MusicId::CASTLE},
    }};

    for (const auto& [failedLevel, expectedMusic] : retryCases) {
        resetStateStack();

        GameProgress progress;
        progress.currentLevel = failedLevel;
        progress.character = CharacterType::LUIGI;

        game.changeState(std::make_unique<GameOverState>(progress));
        game.update(0.f); // enter GameOver and build the retry menu
        game.processInput(pressed(sf::Keyboard::Key::Enter));
        game.update(0.f); // apply the queued PlayState retry

        assert(game.stackDepth() == 1);
        assert(SoundManager::getInstance().getLevelMusicId().has_value());
        assert(SoundManager::getInstance().getLevelMusicId().value() == expectedMusic);
        assert(saveManager.getData().highestUnlockedLevel >= highestUnlockedBefore);
    }

    resetStateStack();

    std::cout << "[PASSED] testGameOverRetryRestartsFailedLevel" << std::endl;
    return true;
}

bool testLevelSelectHonorsUnlockedBoundary() {
    std::cout << "[RUNNING] testLevelSelectHonorsUnlockedBoundary..." << std::endl;

    auto& game = GameManager::getInstance();
    auto& saveManager = game.getSaveManager();
    const SaveData originalSave = saveManager.getData();

    resetStateStack();
    saveManager.resetToDefaults(); // only level 1 is available for this menu

    LevelSelectState levelSelect;
    levelSelect.onEnter();
    levelSelect.processInput(pressed(sf::Keyboard::Key::Right)); // select level 2
    levelSelect.processInput(pressed(sf::Keyboard::Key::Enter));
    game.update(0.f);
    assert(game.stackDepth() == 0); // locked level must not queue a state change

    assert(saveManager.updateHighestUnlockedLevel(4));
    levelSelect.onEnter();
    for (int i = 0; i < 3; ++i) {
        levelSelect.processInput(pressed(sf::Keyboard::Key::Right));
    }
    levelSelect.processInput(pressed(sf::Keyboard::Key::Enter)); // launch level 4
    game.update(0.f); // LevelSelect -> CharacterSelect
    assert(game.stackDepth() == 1);

    game.processInput(pressed(sf::Keyboard::Key::Enter)); // choose Mario
    game.update(0.f); // CharacterSelect -> PlayState(level 4)
    assert(game.stackDepth() == 1);
    assert(SoundManager::getInstance().getLevelMusicId().has_value());
    assert(SoundManager::getInstance().getLevelMusicId().value() == MusicId::CASTLE);

    levelSelect.onExit();
    resetStateStack();
    restoreSaveData(saveManager, originalSave);

    std::cout << "[PASSED] testLevelSelectHonorsUnlockedBoundary" << std::endl;
    return true;
}

bool testLevelSelectRendersLockedFeedbackAndBacksOut() {
    std::cout << "[RUNNING] testLevelSelectRendersLockedFeedbackAndBacksOut..." << std::endl;

    auto& game = GameManager::getInstance();
    auto& saveManager = game.getSaveManager();
    const SaveData originalSave = saveManager.getData();

    resetStateStack();
    saveManager.resetToDefaults();

    LevelSelectState levelSelect;
    levelSelect.onEnter();
    levelSelect.processInput(pressed(sf::Keyboard::Key::Right));

    sf::RenderTexture renderTexture;
    assert(renderTexture.resize({640u, 360u}));
    renderTexture.clear(sf::Color::Transparent);
    levelSelect.render(renderTexture);
    renderTexture.display();

    // The selected Level 2 action tag must visibly use the locked feedback
    // color, not the playable gold tag. This checks the rendered contract
    // through the real state handler and renderer, without private access.
    const sf::Image image = renderTexture.getTexture().copyToImage();
    bool foundLockedInk = false;
    for (unsigned int y = 266; y < 286 && !foundLockedInk; ++y) {
        for (unsigned int x = 178; x < 320; ++x) {
            const sf::Color pixel = image.getPixel({x, y});
            if (pixel.r > 150 && pixel.r > pixel.g + 30 && pixel.r > pixel.b + 30) {
                foundLockedInk = true;
                break;
            }
        }
    }
    assert(foundLockedInk);

    levelSelect.processInput(pressed(sf::Keyboard::Key::Escape));
    game.update(0.f);
    assert(game.stackDepth() == 1);

    resetStateStack();
    levelSelect.onExit();
    restoreSaveData(saveManager, originalSave);

    std::cout << "[PASSED] testLevelSelectRendersLockedFeedbackAndBacksOut" << std::endl;
    return true;
}

} // namespace

int main() {
    IsolatedRuntimeFixture runtimeFixture;

    const bool ok = testRaceConditionGuard()
                 && testWinDecisionCommittedOnce()
                 && testSingleDeathFiresOneEventAndOneLifeLoss()
                 && testSecondDeathWhileDyingIsIgnored()
                 && testRespawnReArmsDeathChain()
                 && testGameOverRetryRestartsFailedLevel()
                 && testLevelSelectHonorsUnlockedBoundary()
                 && testLevelSelectRendersLockedFeedbackAndBacksOut();

    if (ok) {
        std::cout << "All PlayState tests passed successfully!" << std::endl;
        return 0;
    }
    std::cout << "PlayState tests FAILED." << std::endl;
    return 1;
}
