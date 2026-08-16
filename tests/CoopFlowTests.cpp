/**
 * @file CoopFlowTests.cpp
 * @brief Co-op campaign mode (2 PLAYER CO-OP) contract tests:
 *        - the two-player level load spawns a second fighter next to 'M';
 *        - any player's death reloads the whole level (like single player)
 *          and applies the shared team accounting (score/coins summed,
 *          lives = minimum);
 *        - exhausting the shared life pool routes to GameOver, and the
 *          GameProgress carries both characters for the retry;
 *        - the co-op character select advances through the same sequential
 *          phases as the PvP picker;
 *        - the HUD aggregates the second player into the team totals.
 *
 * The runtime is isolated in a temp directory (assets + levels copied,
 * cwd switched) so SaveManager writes never touch the repository.
 */

#include <cassert>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <utility>

#include <SFML/Window/Keyboard.hpp>

#include "entities/Mario.h"
#include "level/Level.h"
#include "level/TileMap.h"
#include "patterns/InputState.h"
#include "states/CoopCharacterSelectState.h"
#include "states/PlayState.h"
#include "ui/HUD.h"

namespace {

constexpr float FRAME_DT = 1.f / 60.f;

std::filesystem::path projectRoot() {
    auto root = std::filesystem::current_path();
    while (!std::filesystem::exists(root / "assets" / "fonts" / "mario.ttf") &&
           root != root.root_path()) {
        root = root.parent_path();
    }
    assert(std::filesystem::exists(root / "assets" / "fonts" / "mario.ttf"));
    return root;
}

/// Copies assets + levels into a temp directory and switches the cwd so the
/// headless session (SaveManager writes, texture/font loads) stays isolated.
class IsolatedRuntimeFixture final {
public:
    IsolatedRuntimeFixture()
        : m_originalDirectory(std::filesystem::current_path()) {
        const auto sourceRoot = projectRoot();
        const auto stamp = std::chrono::high_resolution_clock::now()
                               .time_since_epoch().count();
        m_runtimeRoot = std::filesystem::temp_directory_path() /
                        ("super_mario_coop_flow_tests_" + std::to_string(stamp));
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
        std::filesystem::current_path(m_originalDirectory);
        std::error_code ignored;
        std::filesystem::remove_all(m_runtimeRoot, ignored);
    }

private:
    std::filesystem::path m_originalDirectory;
    std::filesystem::path m_runtimeRoot;
};

/// Synthetic input helper: one clean key press inside a single frame.
class PressedKey {
public:
    explicit PressedKey(sf::Keyboard::Key key) {
        const sf::Event press(sf::Event::KeyPressed{
            key, sf::Keyboard::Scan::Unknown, false, false, false, false});
        m_state.beginFrame();
        m_state.handleEvent(press);
    }

    const InputState& state() const { return m_state; }

private:
    InputState m_state;
};

/// Drive updates until `predicate` holds or `maxFrames` frames elapse.
template <typename State, typename Predicate>
bool waitFor(State& state, Predicate predicate, int maxFrames) {
    for (int frame = 0; frame < maxFrames; ++frame) {
        if (predicate()) {
            return true;
        }
        state.update(FRAME_DT);
    }
    return predicate();
}

bool testCoopLevelLoadSpawnsSecondPlayer() {
    std::cout << "[RUNNING] testCoopLevelLoadSpawnsSecondPlayer..." << std::endl;

    Level level;
    level.setTheme(LevelTheme::OVERWORLD);
    level.setCameraVerticalMode(CameraVerticalMode::LOCKED);
    assert(level.loadFromFile("levels/level1.txt",
                              CharacterType::MARIO, CharacterType::LUIGI));

    assert(level.isCoopMode());
    assert(!level.isPvpMode());
    assert(level.getMario() != nullptr);
    assert(level.getMario2() != nullptr);

    // Both fighters keep their authentic campaign identities.
    assert(level.getMario()->getCharacterType() == CharacterType::MARIO);
    assert(level.getMario2()->getCharacterType() == CharacterType::LUIGI);

    // Player two spawns on the tile next to the shared 'M' spawn.
    const float dx = std::abs(level.getMario2()->getPosition().x -
                              level.getMario()->getPosition().x);
    const float dy = std::abs(level.getMario2()->getPosition().y -
                              level.getMario()->getPosition().y);
    assert(dx <= 33.f);
    assert(dy <= 1.f);

    // Both players share the campaign pit-death contract.
    const float levelHeight =
        static_cast<float>(level.getTileMap().getHeight() * 32);
    assert(level.getMario()->getPosition().y < levelHeight);
    assert(level.getMario2()->getPosition().y < levelHeight);

    // The co-op update path steps the full campaign world for both players.
    for (int frame = 0; frame < 30; ++frame) {
        level.update(FRAME_DT);
    }
    assert(level.getMario2()->isActive() || level.getMario2()->isDying());

    std::cout << "[PASSED] testCoopLevelLoadSpawnsSecondPlayer" << std::endl;
    return true;
}

bool testCoopDeathReloadsLevelWithTeamAccounting() {
    std::cout << "[RUNNING] testCoopDeathReloadsLevelWithTeamAccounting..."
              << std::endl;

    PlayState state(1, CharacterType::MARIO, CharacterType::LUIGI);
    state.onEnter();
    assert(state.isCoopSession());
    assert(state.getLevel() != nullptr);
    assert(state.getLevel()->getMario2() != nullptr);

    Level* levelBefore = state.getLevel();
    levelBefore->getMario()->setScore(1000);
    levelBefore->getMario()->setCoinCount(3);
    levelBefore->getMario2()->setScore(700);
    levelBefore->getMario2()->setCoinCount(4);

    // Player two dies: the whole level must reload (like single player).
    levelBefore->getMario2()->loseLife();
    assert(levelBefore->getMario2()->isDying());

    assert(waitFor(
        state,
        [&state, levelBefore] { return state.getLevel() != levelBefore; },
        600));

    const Level* levelAfter = state.getLevel();
    assert(levelAfter != nullptr);
    assert(levelAfter->isCoopMode());
    assert(levelAfter->getMario() != nullptr);
    assert(levelAfter->getMario2() != nullptr);

    // Team accounting after the reload: score/coins are the summed totals on
    // player one, player two restarts from zero, and the shared pool lost
    // exactly one life for the death.
    assert(levelAfter->getMario()->getScore() == 1700);
    assert(levelAfter->getMario()->getCoinCount() == 7);
    assert(levelAfter->getMario2()->getScore() == 0);
    assert(levelAfter->getMario2()->getCoinCount() == 0);
    assert(levelAfter->getMario()->getLives() == 2);
    assert(levelAfter->getMario2()->getLives() == 2);

    // The reloaded fighters are alive and controllable again.
    assert(!levelAfter->getMario()->isDying());
    assert(!levelAfter->getMario2()->isDying());

    // Character identities survive the reload.
    assert(levelAfter->getMario()->getCharacterType() == CharacterType::MARIO);
    assert(levelAfter->getMario2()->getCharacterType() == CharacterType::LUIGI);

    std::cout << "[PASSED] testCoopDeathReloadsLevelWithTeamAccounting"
              << std::endl;
    return true;
}

bool testCoopGameOverOnExhaustedTeamLives() {
    std::cout << "[RUNNING] testCoopGameOverOnExhaustedTeamLives..." << std::endl;

    PlayState state(2, CharacterType::LUIGI, CharacterType::MARIO);
    state.onEnter();
    assert(state.getLevel() != nullptr);

    Level* level = state.getLevel();
    level->getMario()->setLives(1);
    level->getMario2()->setLives(1);
    level->getMario()->setScore(500);
    level->getMario2()->setScore(250);

    // The last shared life is lost -> GameOver, not a reload.
    level->getMario2()->loseLife();

    // Wait for the death animation / fallback timer to commit the result.
    // The GameOver transition is queued on GameManager and only applied by
    // the game loop, so from the outside the level must NOT be rebuilt.
    const Level* levelBefore = state.getLevel();
    for (int frame = 0; frame < 400; ++frame) {
        state.update(FRAME_DT);
    }
    assert(state.getLevel() == levelBefore);
    assert(state.getProgress().lives == 0);
    assert(state.getProgress().score == 750);
    assert(state.getProgress().isCoop);
    assert(state.getProgress().currentLevel == 2);
    // Both characters survive in the progress so RETRY keeps the team.
    assert(state.getProgress().character == CharacterType::LUIGI);
    assert(state.getProgress().character2 == CharacterType::MARIO);

    std::cout << "[PASSED] testCoopGameOverOnExhaustedTeamLives" << std::endl;
    return true;
}

bool testCoopCharacterSelectSequentialPhases() {
    std::cout << "[RUNNING] testCoopCharacterSelectSequentialPhases..."
              << std::endl;

    CoopCharacterSelectState select(3);
    select.onEnter();
    assert(select.getCurrentPicker() == 0);
    assert(select.getSelectedLevel() == 3);

    // Player one confirms the default card (Mario).
    {
        const PressedKey enter(sf::Keyboard::Key::Enter);
        select.processInput(enter.state());
    }
    assert(select.getCurrentPicker() == 1);
    assert(select.getPlayerOneChoice() == CharacterType::MARIO);

    // ESC during player two's pick rewinds to player one.
    {
        const PressedKey escape(sf::Keyboard::Key::Escape);
        select.processInput(escape.state());
    }
    assert(select.getCurrentPicker() == 0);

    // Player one re-picks Luigi, player two mirrors with Luigi too.
    {
        const PressedKey down(sf::Keyboard::Key::Down);
        select.processInput(down.state());
    }
    {
        const PressedKey enter(sf::Keyboard::Key::Enter);
        select.processInput(enter.state());
    }
    assert(select.getCurrentPicker() == 1);
    assert(select.getPlayerOneChoice() == CharacterType::LUIGI);

    {
        const PressedKey enter(sf::Keyboard::Key::Enter);
        select.processInput(enter.state());
    }
    assert(select.getCurrentPicker() == 2); // confirm splash
    assert(select.getPlayerTwoChoice() == CharacterType::MARIO);

    std::cout << "[PASSED] testCoopCharacterSelectSequentialPhases" << std::endl;
    return true;
}

bool testHUDAggregatesSecondPlayerTotals() {
    std::cout << "[RUNNING] testHUDAggregatesSecondPlayerTotals..." << std::endl;

    Mario playerOne(sf::Vector2f(0.f, 0.f), sf::Vector2f(32.f, 32.f));
    Mario playerTwo(sf::Vector2f(0.f, 0.f), sf::Vector2f(32.f, 32.f));
    playerOne.setCoinCount(3);
    playerTwo.setCoinCount(4);

    HUD hud(playerOne, 1, 1);
    assert(hud.getCoinCount() == 3); // single-player view is unchanged

    hud.attachSecondPlayer(playerTwo);
    assert(hud.getCoinCount() == 7); // team total once the partner attaches

    std::cout << "[PASSED] testHUDAggregatesSecondPlayerTotals" << std::endl;
    return true;
}

} // namespace

int main() {
    const IsolatedRuntimeFixture runtime;

    const bool success = testCoopLevelLoadSpawnsSecondPlayer() &&
                         testCoopDeathReloadsLevelWithTeamAccounting() &&
                         testCoopGameOverOnExhaustedTeamLives() &&
                         testCoopCharacterSelectSequentialPhases() &&
                         testHUDAggregatesSecondPlayerTotals();

    if (success) {
        std::cout << "All CoopFlow tests passed successfully!" << std::endl;
        return 0;
    }

    std::cout << "CoopFlow tests FAILED." << std::endl;
    return 1;
}
