/**
 * @file LevelCatalogTests.cpp
 * @author TV1 (Duong)
 * @brief Headless regression tests for the Sprint 6 level-catalog and
 *        GameProgress contracts (S6-TV1-05 / 06 / 07 / 08).
 *
 * The progression cases use isolated runtime fixtures and public Level/
 * PlayState seams, so they exercise the actual flag/goal and transition paths
 * without depending on an interactive SFML window.
 */

#include <cassert>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "core/GameManager.h"
#include "core/LevelCatalog.h"
#include "core/GameProgress.h"
#include "core/SaveManager.h"
#include "level/Level.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "patterns/IObserver.h"
#include "states/IGameState.h"
#include "states/PlayState.h"

namespace {

class CompletionCounter final : public IObserver {
public:
    CompletionCounter() {
        m_subscription = EventBus::getInstance().subscribe(
            EventType::LEVEL_COMPLETED, this);
    }

    ~CompletionCounter() override = default;

    void onNotify(const GameEvent& eventData) override {
        const EventType event = eventData.type;
        if (event == EventType::LEVEL_COMPLETED) {
            ++count;
        }
    }

    int count = 0;
private:
    Subscription m_subscription;
};

class LevelStartedCounter final : public IObserver {
public:
    LevelStartedCounter() {
        m_subscription = EventBus::getInstance().subscribe(
            EventType::LEVEL_STARTED, this);
    }

    ~LevelStartedCounter() override = default;

    void onNotify(const GameEvent& eventData) override {
        const EventType event = eventData.type;
        if (event == EventType::LEVEL_STARTED) {
            ++count;
        }
    }

    int count = 0;
private:
    Subscription m_subscription;
};

class EmptyState final : public IGameState {
public:
    void onEnter() override {}
    void onExit() override {}
    void processEvents(const sf::Event&) override {}
    void processInput(const InputState&) override {}
    void update(float) override {}
    void render(sf::RenderTarget&) override {}
};

class IsolatedRuntimeFixture final {
public:
    IsolatedRuntimeFixture()
        : m_originalDirectory(std::filesystem::current_path()) {
        std::filesystem::path sourceRoot = m_originalDirectory;
        while (!std::filesystem::exists(sourceRoot / "assets" / "fonts" /
                                         "mario.ttf") &&
               sourceRoot != sourceRoot.root_path()) {
            sourceRoot = sourceRoot.parent_path();
        }
        assert(std::filesystem::exists(sourceRoot / "assets" / "fonts" /
                                       "mario.ttf"));

        const auto stamp = std::chrono::high_resolution_clock::now()
                               .time_since_epoch().count();
        m_runtimeRoot = std::filesystem::temp_directory_path() /
                        ("super_mario_progression_tests_" +
                         std::to_string(stamp));
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

void resetStateStack(GameManager& game) {
    game.changeState(std::make_unique<EmptyState>());
    game.update(0.0f);
    assert(game.stackDepth() == 1);
    game.popState();
    game.update(0.0f);
    assert(game.stackDepth() == 0);
}

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
    assert(catalog[0].initialTheme == LevelTheme::OVERWORLD);
    assert(catalog[0].dominantTheme == LevelTheme::OVERWORLD);
    assert(catalog[0].music == MusicId::OVERWORLD);
    assert(catalog[0].cameraMode == CameraVerticalMode::DEAD_ZONE);

    assert(catalog[1].worldLabel == "1-2");
    assert(catalog[1].filePath == "levels/level2.txt");
    assert(catalog[1].theme == LevelTheme::OVERWORLD);
    assert(catalog[1].initialTheme == LevelTheme::OVERWORLD);
    assert(catalog[1].dominantTheme == LevelTheme::UNDERGROUND);
    assert(catalog[1].music == MusicId::OVERWORLD);
    assert(catalog[1].cameraMode == CameraVerticalMode::DEAD_ZONE);

    assert(catalog[2].worldLabel == "1-3");
    assert(catalog[2].filePath == "levels/level3.txt");
    assert(catalog[2].theme == LevelTheme::OVERWORLD);
    assert(catalog[2].initialTheme == LevelTheme::OVERWORLD);
    assert(catalog[2].dominantTheme == LevelTheme::UNDERWATER);
    assert(catalog[2].music == MusicId::OVERWORLD);
    assert(catalog[2].cameraMode == CameraVerticalMode::DEAD_ZONE);

    assert(catalog[3].worldLabel == "1-4");
    assert(catalog[3].filePath == "levels/level4.txt");
    assert(catalog[3].theme == LevelTheme::CASTLE);
    assert(catalog[3].initialTheme == LevelTheme::CASTLE);
    assert(catalog[3].dominantTheme == LevelTheme::CASTLE);
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

bool testLevel3CompletionUsesActualFlagSequence() {
    std::cout << "[RUNNING] testLevel3CompletionUsesActualFlagSequence..."
              << std::endl;

    const std::filesystem::path originalDirectory =
        std::filesystem::current_path();
    std::filesystem::path projectRoot = originalDirectory;
    while (!std::filesystem::exists(projectRoot / "levels" / "level3.txt") &&
           projectRoot != projectRoot.root_path()) {
        projectRoot = projectRoot.parent_path();
    }
    assert(std::filesystem::exists(projectRoot / "levels" / "level3.txt"));
    std::filesystem::current_path(projectRoot);

    // This drives the public gameplay seam (Mario's position) into the real
    // Level::checkFinishFlag path.  Completion is observed from the event bus;
    // no LEVEL_COMPLETED event is injected by the test.
    CompletionCounter counter;
    Level level;
    level.setTheme(LevelCatalog::find(3)->initialTheme);
    assert(level.loadFromFile(LevelCatalog::find(3)->filePath));

    const auto finishTiles = level.getTileMap().findTiles('F');
    assert(finishTiles.size() == 1);
    level.getMario()->setPosition(
        TileMap::gridToWorldPosition(finishTiles.front()) +
        sf::Vector2f(7.0f, 8.0f));
    level.update(0.0f);

    assert(level.isFlagSequenceActive());
    assert(!level.isLevelCompleted());
    for (int frame = 0; frame < 60 && !level.isLevelCompleted(); ++frame) {
        level.update(1.0f);
    }

    assert(level.isLevelCompleted());
    assert(level.getTileMap().isFlagFullyDropped());
    assert(counter.count == 1);

    // Repeated update frames cannot re-emit the one-shot completion.
    level.update(1.0f);
    assert(counter.count == 1);

    std::filesystem::current_path(originalDirectory);

    std::cout << "[PASSED] testLevel3CompletionUsesActualFlagSequence"
              << std::endl;
    return true;
}

void completePlayStateFlagLevel(PlayState& state) {
    const int levelNumber = state.getProgress().currentLevel;
    assert(levelNumber >= 1 && levelNumber <= 3);
    Level* levelBefore = state.getLevel();
    assert(levelBefore != nullptr);

    const auto finishTiles = levelBefore->getTileMap().findTiles('F');
    assert(finishTiles.size() == 1);
    levelBefore->getMario()->setPosition(
        TileMap::gridToWorldPosition(finishTiles.front()) +
        sf::Vector2f(7.0f, 8.0f));
    state.update(0.0f);
    assert(levelBefore->isFlagSequenceActive());

    for (int frame = 0;
         frame < 60 && state.getProgress().currentLevel == levelNumber;
         ++frame) {
        state.update(1.0f);
    }
    assert(state.getProgress().currentLevel == levelNumber + 1);

    // Complete the real PlayState fade/load/fade-in transaction.  The level
    // is replaced only by the production transition state machine.
    state.update(0.5f);
    state.update(0.0f);
    assert(state.getLevel() != nullptr);
    assert(state.getLevel() != levelBefore);
    state.update(0.0f); // consume the post-load delta guard
    state.update(0.5f);
}

void completePlayStateCastleLevel(PlayState& state) {
    assert(state.getProgress().currentLevel == 4);
    Level* level = state.getLevel();
    assert(level != nullptr);
    Mario* mario = level->getMario();
    assert(mario != nullptr);

    const Entity* axe = nullptr;
    const Entity* toad = nullptr;
    for (const Entity& entity : level->getEntities()) {
        if (entity.isBowserAxe()) {
            axe = &entity;
        }
        if (entity.isToad()) {
            toad = &entity;
        }
    }
    assert(axe != nullptr);
    assert(toad != nullptr);

    mario->setInvincible(30.0f);
    mario->setPosition(axe->getPosition());
    state.update(0.0f);
    assert(!level->isLevelCompleted());

    const auto spawns = level->getTileMap().findTiles('M');
    assert(spawns.size() == 1);
    mario->setPosition(TileMap::gridToWorldPosition(spawns.front()));
    for (int frame = 0; frame < 40; ++frame) {
        state.update(0.1f);
    }
    assert(level->getTileMap().findTiles('=').empty());
    assert(!level->isLevelCompleted());

    mario->setPosition(toad->getPosition());
    state.update(0.0f);
    assert(!level->isLevelCompleted());
    state.update(3.0f);
    assert(state.getProgress().currentLevel == 5);
}

InputState enterPressed() {
    InputState input;
    input.beginFrame();
    input.handleEvent(sf::Event(sf::Event::KeyPressed{
        sf::Keyboard::Key::Enter, sf::Keyboard::Scan::Unknown,
        false, false, false, false}));
    return input;
}

bool testRealProgressionSaveReplayAndCoopCompletion() {
    std::cout << "[RUNNING] testRealProgressionSaveReplayAndCoopCompletion..."
              << std::endl;

    IsolatedRuntimeFixture runtime;
    GameManager& game = GameManager::getInstance();
    resetStateStack(game);
    SaveManager& saveManager = game.getSaveManager();
    saveManager.resetToDefaults();
    assert(saveManager.save());

    CompletionCounter counter;
    LevelStartedCounter startedCounter;
    PlayState state(1, CharacterType::MARIO);
    state.onEnter();
    assert(state.getLevel() != nullptr);
    assert(startedCounter.count == 1);

    // Reach Levels 2, 3 and 4 through the actual flag sequence.  Every
    // completion advances PlayState and writes the unlock before the next
    // level loads; a fresh SaveManager proves the durable file boundary.
    for (int expectedLevel = 1; expectedLevel <= 3; ++expectedLevel) {
        assert(state.getProgress().currentLevel == expectedLevel);
        completePlayStateFlagLevel(state);
        assert(startedCounter.count == expectedLevel + 1);

        SaveManager freshLoad(saveManager.getSavePath());
        assert(freshLoad.load());
        assert(freshLoad.getData().highestUnlockedLevel == expectedLevel + 1);
    }
    assert(counter.count == 3);
    assert(state.getProgress().currentLevel == 4);

    completePlayStateCastleLevel(state);
    assert(counter.count == 4);
    assert(state.getProgress().currentLevel == 5);

    SaveManager finalLoad(saveManager.getSavePath());
    assert(finalLoad.load());
    assert(finalLoad.getData().highestUnlockedLevel == LevelCatalog::count());

    // Let PlayState commit the terminal transition.  The GameManager then
    // enters the real WinState.  No LEVEL_STARTED event is emitted while the
    // terminal state is active, and its one-state stack is externally visible.
    state.onExit();
    state.update(0.5f);
    state.update(0.0f);
    game.update(0.0f);
    assert(game.stackDepth() == 1);
    assert(startedCounter.count == 4);

    // WinState's first action is PLAY AGAIN after the terminal level.  It
    // starts a fresh Level 1 while the durable unlock remains at Level 4.
    game.processInput(enterPressed());
    game.update(0.0f);
    assert(game.stackDepth() == 1);
    assert(startedCounter.count == 5);
    SaveManager replayLoad(saveManager.getSavePath());
    assert(replayLoad.load());
    assert(replayLoad.getData().highestUnlockedLevel == LevelCatalog::count());

    // Co-op players overlap the same goal window on adjacent positions.  The
    // first player claims the one-shot sequence and the team applies exactly
    // one level transition despite both reaching it in the same update.
    resetStateStack(game);
    CompletionCounter coopCounter;
    PlayState coop(1, CharacterType::MARIO, CharacterType::LUIGI);
    coop.onEnter();
    Level* coopLevel = coop.getLevel();
    assert(coopLevel != nullptr && coopLevel->getMario2() != nullptr);
    const auto coopFinish = coopLevel->getTileMap().findTiles('F');
    assert(coopFinish.size() == 1);
    const sf::Vector2f finishPosition =
        TileMap::gridToWorldPosition(coopFinish.front());
    coopLevel->getMario()->setPosition(finishPosition + sf::Vector2f(7.f, 8.f));
    coopLevel->getMario2()->setPosition(finishPosition + sf::Vector2f(15.f, 8.f));
    coop.update(0.0f);
    assert(coopLevel->isFlagSequenceActive());
    for (int frame = 0;
         frame < 60 && coop.getProgress().currentLevel == 1;
         ++frame) {
        coop.update(1.0f);
    }
    assert(coop.getProgress().currentLevel == 2);
    assert(coopCounter.count == 1);
    coop.update(1.0f);
    assert(coopCounter.count == 1);
    coop.onExit();

    resetStateStack(game);
    std::cout << "[PASSED] testRealProgressionSaveReplayAndCoopCompletion"
              << std::endl;
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
                 && testLevelSpawnHonorsCharacterIdentity()
                 && testLevel3CompletionUsesActualFlagSequence()
                 && testRealProgressionSaveReplayAndCoopCompletion();

    if (ok) {
        std::cout << "All LevelCatalogTests passed successfully!" << std::endl;
        return 0;
    }
    std::cout << "LevelCatalogTests FAILED." << std::endl;
    return 1;
}
