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
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include <SFML/Window/Keyboard.hpp>

#include "entities/Mario.h"
#include "level/Level.h"
#include "level/TileMap.h"
#include "patterns/InputState.h"
#include "physics/PhysicsEngine.h"
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

bool testCoopPlayersClampedInsideCameraViewport() {
    std::cout << "[RUNNING] testCoopPlayersClampedInsideCameraViewport..." << std::endl;

    Level level;
    if (!level.loadFromFile("levels/level1.txt", CharacterType::MARIO, CharacterType::LUIGI)) {
        std::cerr << "Failed to load coop level1.txt" << std::endl;
        return false;
    }

    // Co-op mode must disable horizontal deadzone to prevent camera freezing
    assert(level.getCamera().getHorizontalDeadzoneRatio() == 0.0f);
    assert(level.getCamera().getVerticalMode() == CameraVerticalMode::DEAD_ZONE);

    Mario* p1 = level.getMario();
    Mario* p2 = level.getMario2();
    assert(p1 != nullptr && p2 != nullptr);

    p1->activateStarman(10.0f);
    p2->activateStarman(10.0f);

    const float initialViewCenter = level.getCamera().getView().getCenter().x;

    // Run Player 1 continuously to the right while Player 2 stands still
    for (int frame = 0; frame < 300; ++frame) {
        p1->moveRight();
        level.update(FRAME_DT);

        // Verify camera horizontal deadzone ratio remains 0.0f in co-op
        assert(level.getCamera().getHorizontalDeadzoneRatio() == 0.0f);

        const sf::View& view = level.getCamera().getView();
        const float viewLeft = view.getCenter().x - (view.getSize().x / 2.0f);
        const float viewRight = view.getCenter().x + (view.getSize().x / 2.0f);

        const float p1Left = p1->getPosition().x;
        const float p1Right = p1Left + p1->getSize().x;
        const float p2Left = p2->getPosition().x;
        const float p2Right = p2Left + p2->getSize().x;

        // Player 1 must not exceed the right edge of the screen
        assert(p1Right <= viewRight + 1.0f);
        // Player 1 must not fall behind the left edge of the screen
        assert(p1Left >= viewLeft - 1.0f);

        // Player 2 must not fall behind the left edge of the screen
        assert(p2Left >= viewLeft - 1.0f);
        // Player 2 must not exceed the right edge of the screen
        assert(p2Right <= viewRight + 1.0f);

        // Verify syncPhysics() synchronizes Box2D body coordinates to entity position
        if (p1->getBody()) {
            const float p1BodyX = PhysicsEngine::metersToPixels(p1->getBody()->GetPosition().x) - (p1->getSize().x / 2.0f);
            assert(std::abs(p1->getPosition().x - p1BodyX) < 0.01f);
        }
        if (p2->getBody()) {
            const float p2BodyX = PhysicsEngine::metersToPixels(p2->getBody()->GetPosition().x) - (p2->getSize().x / 2.0f);
            assert(std::abs(p2->getPosition().x - p2BodyX) < 0.01f);
        }
    }

    // Moving forward while players are at opposite boundaries advances the camera smoothly without freezing
    assert(level.getCamera().getView().getCenter().x > initialViewCenter + 50.0f);

    std::cout << "[PASSED] testCoopPlayersClampedInsideCameraViewport" << std::endl;
    return true;
}

// Co-op shared-camera contract (2026-08-23 design):
//  1. Vertical TRACK mode pans the view to a fit-both biased midpoint with
//     edge-margin follow: the shared view only scrolls when a partner would
//     otherwise leave the frame, so hop jitter and mid-frame elevations never
//     drag the view around.
//  2. Horizontal edge-stuck rule still pins a player at the screen edge when
//     the partners try to separate wider than one viewport.
//
// The vertical phases need a level tall enough to leave the shared view real
// panning room, so this test builds its own flat synthetic fixture instead of
// relying on a shipped level whose trimmed height can be near one viewport.
bool testCoopCameraTracksVerticalMidpointAndSticksAtEdges() {
    std::cout << "[RUNNING] testCoopCameraTracksVerticalMidpointAndSticksAtEdges..."
              << std::endl;

    // Flat 70x30 level: thick floor across rows 12-13, flagpole at column 68,
    // spawn 'M' at (10,11). No enemies, no pits, plenty of sky to pan through.
    const int kCols = 70;
    const int kRows = 30;
    const auto fixturePath =
        std::filesystem::temp_directory_path() / "mario_coop_camera_level.txt";
    {
        std::ofstream out(fixturePath);
        assert(out.is_open());
        out << "# Temporary fixture: tall flat co-op camera level\n";
        for (int row = 0; row < kRows; ++row) {
            std::string line(kCols, '.');
            if (row == 8) {
                line[68] = 'T';
            } else if (row == 9) {
                line[68] = 'F';
            } else if (row == 10 || row == 11) {
                line[68] = '|';
            } else if (row == 12 || row == 13) {
                line = std::string(kCols, '0');
            }
            if (row == 11) {
                line[10] = 'M';
            }
            out << line << '\n';
        }
    }

    Level level;
    if (!level.loadFromFile(fixturePath.string(), CharacterType::MARIO,
                            CharacterType::LUIGI)) {
        std::cerr << "Failed to load coop camera fixture" << std::endl;
        return false;
    }

    Mario* p1 = level.getMario();
    Mario* p2 = level.getMario2();
    assert(p1 != nullptr && p2 != nullptr);

    // Split the partners vertically: P1 parked high with gravity disabled,
    // P2 standing on the floor. The camera must pan UP toward their midpoint.
    p1->setPosition(TileMap::gridToWorldPosition(sf::Vector2i{5, 0}));
    if (b2Body* b1 = p1->getBody()) {
        b1->SetGravityScale(0.f);
        b1->SetLinearVelocity(b2Vec2(0.f, 0.f));
    }
    p1->setVelocity({0.f, 0.f});
    p2->setPosition(TileMap::gridToWorldPosition(sf::Vector2i{30, 11}));
    for (int frame = 0; frame < 60; ++frame) {
        level.update(FRAME_DT);
    }

    const float elevatedCenterY =
        level.getCamera().getView().getCenter().y;

    // The split must have dragged the shared view far up from its initial
    // bottom-clamped resting position to keep both partners framed.
    constexpr int FIXTURE_TILE_PX = 32;
    const float bottomRestY =
        static_cast<float>(kRows * FIXTURE_TILE_PX) -
        level.getCamera().getView().getSize().y / 2.f;
    assert(elevatedCenterY < bottomRestY - 200.f);

    // Bring P1 down onto the floor next to P2: under DEAD_ZONE mode
    // when P1 returns down to the floor, the view tracks downward toward the players.
    if (b2Body* b1 = p1->getBody()) {
        b1->SetGravityScale(1.0f);
    }
    p1->setPosition(TileMap::gridToWorldPosition(sf::Vector2i{19, 11}));
    for (int frame = 0; frame < 90; ++frame) {
        level.update(FRAME_DT);
    }
    const float groundedCenterY = level.getCamera().getView().getCenter().y;
    assert(groundedCenterY > elevatedCenterY + 100.f);

    // Both players must be visible inside the shared view at all times here.
    {
        const sf::View& view = level.getCamera().getView();
        const float left = view.getCenter().x - view.getSize().x / 2.f;
        const float right = view.getCenter().x + view.getSize().x / 2.f;
        const float top = view.getCenter().y - view.getSize().y / 2.f;
        const float bottom = view.getCenter().y + view.getSize().y / 2.f;
        for (const Mario* player : {p1, p2}) {
            assert(player->getPosition().x >= left - 1.f &&
                   player->getPosition().x + player->getSize().x <=
                       right + 1.f);
            assert(player->getPosition().y + player->getSize().y > top - 8.f);
            assert(player->getPosition().y < bottom + 8.f);
        }
    }

    // Edge-stuck: park idle P2 beside P1, then run P1 right along the long
    // floor until the separation limit pins P2 at the view's LEFT edge while
    // the camera keeps following P1.
    p1->setPosition(TileMap::gridToWorldPosition(sf::Vector2i{40, 11}));
    p2->setPosition(TileMap::gridToWorldPosition(sf::Vector2i{20, 11}));
    for (int frame = 0; frame < 30; ++frame) {
        level.update(FRAME_DT);
    }
    for (int frame = 0; frame < 300; ++frame) {
        p1->setMoveIntent(1.0f);
        level.update(FRAME_DT);
    }
    p1->stopMoving();

    const sf::View& view = level.getCamera().getView();
    const float viewLeft = view.getCenter().x - view.getSize().x / 2.f;
    const float viewRight = view.getCenter().x + view.getSize().x / 2.f;
    // P2 is pinned at the left edge
    assert(p2->getPosition().x >= viewLeft - 1.f);
    assert(p2->getPosition().x <= viewLeft + 16.f);
    // P1 is bounded at the right edge
    assert(p1->getPosition().x + p1->getSize().x <= viewRight + 1.f);
    // Camera center tracks horizontal midpoint of both players
    const float expectedCenter =
        (p1->getPosition().x + p1->getSize().x / 2.f +
         p2->getPosition().x + p2->getSize().x / 2.f) / 2.f;
    assert(std::abs(view.getCenter().x - expectedCenter) <= 5.f);

    std::cout << "[PASSED] testCoopCameraTracksVerticalMidpointAndSticksAtEdges"
              << std::endl;
    return true;
}

bool testCoopDualPipeWarpRequiresBothPlayers() {
    std::cout << "[RUNNING] testCoopDualPipeWarpRequiresBothPlayers..." << std::endl;
    Level level;
    assert(level.loadFromFile("levels/level1.txt", CharacterType::MARIO, CharacterType::LUIGI));
    Mario* p1 = level.getMario();
    Mario* p2 = level.getMario2();
    assert(p1 != nullptr && p2 != nullptr);

    const auto& warpEntries = level.getTileMap().getWarpEntries();
    assert(!warpEntries.empty());
    const auto& entry = warpEntries.front();
    const sf::Vector2f pipeWorld = TileMap::gridToWorldPosition(entry.position);

    // 1. Move P1 to pipe top, P2 stands on ground beside the pipe -> Warp should NOT trigger
    p1->setPosition(sf::Vector2f(pipeWorld.x + 8.f, pipeWorld.y - p1->getSize().y));
    p2->setPosition(sf::Vector2f(pipeWorld.x - 40.f, pipeWorld.y)); // P2 is near but not on the pipe
    for (int frame = 0; frame < 10; ++frame) {
        p1->setVerticalIntent(1.0f);
        p2->setVerticalIntent(0.0f); // P2 is not pressing down
        level.update(FRAME_DT);
    }
    assert(!level.isPipeWarpActive());

    // 2. Move P2 onto the pipe as well, both press Down -> Pipe warp DOES trigger
    p1->setPosition(sf::Vector2f(pipeWorld.x + 8.f, pipeWorld.y - p1->getSize().y));
    p2->setPosition(sf::Vector2f(pipeWorld.x + 24.f, pipeWorld.y - p2->getSize().y));
    for (int frame = 0; frame < 20; ++frame) {
        p1->setVerticalIntent(1.0f);
        p2->setVerticalIntent(1.0f);
        level.update(FRAME_DT);
    }
    assert(level.isPipeWarpActive());

    // 3. Update until warp finishes -> Both players emerged at the destination
    for (int frame = 0; frame < 120 && level.isPipeWarpActive(); ++frame) {
        level.update(FRAME_DT);
    }
    assert(!level.isPipeWarpActive());
    assert(p1->getPosition().x > pipeWorld.x + 200.f);
    assert(p2->getPosition().x > pipeWorld.x + 200.f);

    std::cout << "[PASSED] testCoopDualPipeWarpRequiresBothPlayers" << std::endl;
    return true;
}

bool testCoopDualFlagpoleCompletion() {
    std::cout << "[RUNNING] testCoopDualFlagpoleCompletion..." << std::endl;
    Level level;
    assert(level.loadFromFile("levels/level1.txt", CharacterType::MARIO, CharacterType::LUIGI));
    Mario* p1 = level.getMario();
    Mario* p2 = level.getMario2();
    assert(p1 != nullptr && p2 != nullptr);

    const auto finishTiles = level.getTileMap().findTiles('F');
    assert(!finishTiles.empty());
    const sf::Vector2f finishPos = TileMap::gridToWorldPosition(finishTiles.front());

    // 1. Only P1 reaches finish flagpole -> Flag sequence should NOT start
    p1->setPosition(finishPos + sf::Vector2f(4.f, 4.f));
    p2->setPosition(finishPos - sf::Vector2f(300.f, 0.f)); // P2 far away
    level.update(FRAME_DT);
    assert(!level.isFlagSequenceActive());
    assert(!level.isLevelCompleted());

    // 2. P2 also reaches finish flagpole -> Flag sequence starts
    p2->setPosition(finishPos + sf::Vector2f(12.f, 4.f));
    level.update(FRAME_DT);
    assert(level.isFlagSequenceActive());

    // 3. Update until both slide and walk into the castle -> Level is completed
    for (int frame = 0; frame < 300 && !level.isLevelCompleted(); ++frame) {
        level.update(FRAME_DT);
    }
    assert(level.isLevelCompleted());

    std::cout << "[PASSED] testCoopDualFlagpoleCompletion" << std::endl;
    return true;
}

// Co-op invisible-ceiling contract (2026-08-23 camera feel pass): the shared
// view can only frame partners closer than one viewport height. Once the
// vertical gap reaches that limit, the higher climber's ascent must stop so
// neither partner can be scrolled out of the frame. Falling is never
// interrupted and nobody is teleported.
bool testCoopInvisibleCeilingCapsVerticalSeparation() {
    std::cout << "[RUNNING] testCoopInvisibleCeilingCapsVerticalSeparation..."
              << std::endl;

    Level level;
    if (!level.loadFromFile("levels/level0.txt", CharacterType::MARIO,
                            CharacterType::LUIGI)) {
        std::cerr << "Failed to load coop level0.txt" << std::endl;
        return false;
    }

    Mario* p1 = level.getMario();
    Mario* p2 = level.getMario2();
    assert(p1 != nullptr && p2 != nullptr);

    // Park P2 on open ground with gravity disabled, then keep pushing P1
    // upward past one viewport: the ceiling guard must cancel the ascent.
    p2->setPosition(TileMap::gridToWorldPosition(sf::Vector2i{30, 10}));
    if (b2Body* b2 = p2->getBody()) {
        b2->SetGravityScale(0.f);
        b2->SetLinearVelocity(b2Vec2(0.f, 0.f));
    }
    p2->setVelocity({0.f, 0.f});
    p1->setPosition(TileMap::gridToWorldPosition(sf::Vector2i{30, 4}));

    const float viewHeight =
        level.getCamera().getView().getSize().y;
    const float maxSeparation = viewHeight - 32.f; // 2 * COOP_VIEW_EDGE_MARGIN
    constexpr float ASCENT_SPEED = 300.f;

    float worstGap = 0.f;
    for (int frame = 0; frame < 240; ++frame) {
        if (b2Body* b1 = p1->getBody()) {
            // Re-assert an upward velocity every frame, as a held jump would.
            b1->SetLinearVelocity(b2Vec2(0.f, -PhysicsEngine::pixelsToMeters(ASCENT_SPEED)));
        }
        level.update(FRAME_DT);
        const float gap = (p1->getPosition().y + p1->getSize().y / 2.f) -
                          (p2->getPosition().y + p2->getSize().y / 2.f);
        worstGap = std::max(worstGap, gap);
    }

    // The climber is stopped just past the limit (one-frame overshoot), and
    // gravity keeps the gap bounded afterwards.
    assert(worstGap <= maxSeparation + 24.f);

    // Both partners stay framed: P2 never leaves through the view bottom.
    const sf::View& view = level.getCamera().getView();
    assert(p2->getPosition().y < view.getCenter().y + view.getSize().y / 2.f + 8.f);

    std::cout << "[PASSED] testCoopInvisibleCeilingCapsVerticalSeparation"
              << std::endl;
    return true;
}

bool testCoopHeadBounceBoostJump() {
    std::cout << "[RUNNING] testCoopHeadBounceBoostJump..." << std::endl;
    CollisionManager::clearPendingPvpHits();

    Level level;
    level.setTheme(LevelTheme::OVERWORLD);
    assert(level.loadFromFile("levels/level1.txt",
                              CharacterType::MARIO, CharacterType::LUIGI));

    Mario* p1 = level.getMario();
    Mario* p2 = level.getMario2();
    assert(p1 && p2);

    b2Body* b1 = p1->getBody();
    b2Body* b2 = p2->getBody();
    assert(b1 && b2);

    p1->setFixtureCollisionGroup(static_cast<int16_t>(-1));
    p2->setFixtureCollisionGroup(static_cast<int16_t>(-2));

    b1->SetTransform(b2Vec2(4.f, 10.f - 0.42f), 0.f);
    b2->SetTransform(b2Vec2(4.f, 10.f), 0.f);
    b1->SetLinearVelocity(b2Vec2(0.f, 5.f));
    b2->SetLinearVelocity(b2Vec2(0.f, 0.f));

    level.update(FRAME_DT);

    // P1 receives upward boost bounce
    assert(b1->GetLinearVelocity().y < 0.f);

    // P2 is unharmed and fully alive
    assert(!p2->isDying());
    assert(p2->getLives() == 3);

    std::cout << "[PASSED] testCoopHeadBounceBoostJump" << std::endl;
    return true;
}

} // namespace

int main() {
    const IsolatedRuntimeFixture runtime;

    const bool success = testCoopLevelLoadSpawnsSecondPlayer() &&
                         testCoopDeathReloadsLevelWithTeamAccounting() &&
                         testCoopGameOverOnExhaustedTeamLives() &&
                         testCoopCharacterSelectSequentialPhases() &&
                         testHUDAggregatesSecondPlayerTotals() &&
                         testCoopPlayersClampedInsideCameraViewport() &&
                         testCoopCameraTracksVerticalMidpointAndSticksAtEdges() &&
                         testCoopInvisibleCeilingCapsVerticalSeparation() &&
                         testCoopDualPipeWarpRequiresBothPlayers() &&
                         testCoopDualFlagpoleCompletion() &&
                         testCoopHeadBounceBoostJump();

    if (success) {
        std::cout << "All CoopFlow tests passed successfully!" << std::endl;
        return 0;
    }

    std::cout << "CoopFlow tests FAILED." << std::endl;
    return 1;
}
