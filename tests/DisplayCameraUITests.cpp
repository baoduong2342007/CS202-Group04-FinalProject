/**
 * @file DisplayCameraUITests.cpp
 * @author TV2
 * @brief Integer viewport, mouse remap, camera policy and UI mouse regression.
 */

#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <string>

#include "core/DisplayConfig.h"
#include "core/SpriteFrames_ovw.h"
#include "core/SpriteFrames_udg.h"
#include "core/SpriteFrames_udw.h"
#include "core/SpriteFrames_castle.h"
#include "core/SpriteFrames_shared.h"
#include "entities/Mario.h"
#include "physics/PhysicsEngine.h"
#include "level/TileFrames.h"
#include "level/Camera.h"
#include "level/TileMap.h"
#include "level/Level.h"
#include "ui/UIMenuWidget.h"
#include "states/LevelSelectState.h"
#include "states/MenuState.h"
#include "states/PauseState.h"
#include "states/CharacterSelectState.h"

namespace {
bool close(float left, float right) {
    return std::abs(left - right) < 0.01f;
}

void testIntegerViewportMatrix() {
    struct Case {
        sf::Vector2u window;
        unsigned int scale;
        sf::Vector2i position;
        sf::Vector2u size;
    };
    const Case cases[] = {
        {{1280u, 720u}, 2u, {0, 0}, {1280u, 720u}},
        {{1600u, 900u}, 2u, {160, 90}, {1280u, 720u}},
        {{1920u, 1080u}, 3u, {0, 0}, {1920u, 1080u}},
        {{1024u, 768u}, 1u, {192, 204}, {640u, 360u}},
    };
    for (const Case& test : cases) {
        const auto viewport =
            DisplayConfig::calculateIntegerViewport(test.window);
        assert(viewport.scale == test.scale);
        assert(viewport.position == test.position);
        assert(viewport.size == test.size);
        const sf::Vector2i physicalCenter{
            viewport.position.x + static_cast<int>(viewport.size.x / 2u),
            viewport.position.y + static_cast<int>(viewport.size.y / 2u)};
        const auto logicalCenter = DisplayConfig::mapPhysicalToLogical(
            physicalCenter, test.window);
        assert(logicalCenter == sf::Vector2i(320, 180));
    }

    assert(!DisplayConfig::mapPhysicalToLogical({0, 0}, {1024u, 768u}));
    assert(DisplayConfig::mapPhysicalToLogical({192, 204}, {1024u, 768u}) ==
           sf::Vector2i(0, 0));
    assert(!DisplayConfig::mapPhysicalToLogical({832, 564}, {1024u, 768u}));

    // The OS-level minimum normally prevents this size, but minimized or
    // programmatic windows must not trigger unsigned offset underflow.
    const auto cropped = DisplayConfig::calculateIntegerViewport({320u, 180u});
    assert(cropped.scale == 1u);
    assert(cropped.position == sf::Vector2i(-160, -90));
    assert(DisplayConfig::mapPhysicalToLogical({160, 90}, {320u, 180u}) ==
           sf::Vector2i(320, 180));
}

void assertViewInside(const Camera& camera, const sf::FloatRect& bounds) {
    const sf::View& view = camera.getView();
    const sf::Vector2f half = view.getSize() / 2.f;
    const sf::Vector2f center = view.getCenter();
    assert(center.x - half.x >= bounds.position.x - 0.01f);
    assert(center.y - half.y >= bounds.position.y - 0.01f);
    assert(center.x + half.x <= bounds.position.x + bounds.size.x + 0.01f);
    assert(center.y + half.y <= bounds.position.y + bounds.size.y + 0.01f);
}

void testCameraPoliciesAndFourEdgeClamp() {
    const sf::FloatRect releaseBounds({0.f, 0.f}, {2000.f, 480.f});
    Camera locked;
    locked.setVerticalMode(CameraVerticalMode::LOCKED);
    locked.init({640.f, 360.f}, releaseBounds);
    const float lockedY = locked.getView().getCenter().y;
    locked.update(0.f, {900.f, 20.f});
    assert(close(locked.getView().getCenter().y, lockedY));
    assertViewInside(locked, releaseBounds);
    locked.update(0.f, {-500.f, -500.f});
    assertViewInside(locked, releaseBounds);
    locked.shake(1.f, 500.f);
    locked.update(0.1f, {-500.f, -500.f});
    assertViewInside(locked, releaseBounds);
    locked.update(0.f, {5000.f, 5000.f});
    assertViewInside(locked, releaseBounds);

    const sf::FloatRect tallBounds({0.f, 0.f}, {1000.f, 1200.f});
    Camera tall;
    tall.setVerticalMode(CameraVerticalMode::DEAD_ZONE);
    tall.init({640.f, 360.f}, tallBounds);
    tall.update(0.f, {500.f, 100.f});
    assert(close(tall.getView().getCenter().y, 248.f));
    assertViewInside(tall, tallBounds);
    tall.update(0.f, {500.f, 1150.f});
    assert(close(tall.getView().getCenter().y, 1020.f));
    assertViewInside(tall, tallBounds);

    Camera small;
    small.init({640.f, 360.f}, {{200.f, 100.f}, {300.f, 200.f}});
    small.update(0.f, {-1000.f, -1000.f});
    assert(small.getView().getCenter() == sf::Vector2f(350.f, 200.f));
}

void testCameraFollowsMarioAndReturnsToOriginalView() {
    const sf::FloatRect bounds({0.f, 0.f}, {2000.f, 1200.f});
    Camera cam;
    cam.setVerticalMode(CameraVerticalMode::DEAD_ZONE);
    cam.init({640.f, 360.f}, bounds);

    // The initial view rests against the bottom of the level. Mario is outside
    // the 32-pixel trigger zone, so the original camera must hold.
    cam.update(0.f, {1000.f, 900.f});
    const float restingY = cam.getView().getCenter().y;
    assert(close(restingY, 1020.f));
    assertViewInside(cam, bounds);

    // At exactly 32 pixels below the original top edge, the camera is still
    // at its original position when Mario returns to this threshold.
    cam.update(0.f, {1000.f, 872.f});
    assert(close(cam.getView().getCenter().y, restingY));

    // One pixel above the return threshold starts vertical following. Mario is
    // kept 32 pixels below the current top edge.
    cam.update(0.f, {1000.f, 871.f});
    const float followedY = cam.getView().getCenter().y;
    assert(close(followedY, 1019.f));
    assert(followedY < restingY);
    assertViewInside(cam, bounds);

    // A higher jump continues to move the camera while preserving the 32px
    // top buffer.
    cam.update(0.f, {1000.f, 800.f});
    assert(close(cam.getView().getCenter().y, 948.f));
    assert(close(cam.getView().getCenter().y - 180.f, 768.f));
    assertViewInside(cam, bounds);

    // Once Mario returns to the original 32px threshold, the camera snaps back
    // to the original view instead of remaining at the high-jump position.
    cam.update(0.f, {1000.f, 872.f});
    assert(close(cam.getView().getCenter().y, restingY));
    cam.update(0.f, {1000.f, 900.f});
    assert(close(cam.getView().getCenter().y, restingY));
    assertViewInside(cam, bounds);
}

void testMenuWidgetLogicalMouseClick() {
    sf::Font font;
    assert(font.openFromFile("assets/fonts/mario.ttf"));
    UIMenuWidget menu(font);
    int first = 0;
    int second = 0;
    menu.addItem("FIRST", [&first] { ++first; });
    menu.addItem("SECOND", [&second] { ++second; });
    menu.setPosition({320.f, 180.f}, UIAnchor::Center);

    const auto secondBounds = menu.getItemBounds(1);
    assert(secondBounds);
    const sf::Vector2i click{
        static_cast<int>(secondBounds->position.x + secondBounds->size.x / 2.f),
        static_cast<int>(secondBounds->position.y + secondBounds->size.y / 2.f)};
    menu.processEvents(sf::Event(sf::Event::MouseMoved{click}));
    menu.processEvents(sf::Event(sf::Event::MouseButtonPressed{
        sf::Mouse::Button::Left, click}));
    assert(first == 0);
    assert(second == 1);
}

void testLevelSelectStateRenderSnapshot() {
    LevelSelectState state;
    state.onEnter();

    sf::RenderTexture rt({DisplayConfig::LOGICAL_WIDTH, DisplayConfig::LOGICAL_HEIGHT});
    state.render(rt);
    rt.display();
}

void testMenuStateRenderSnapshot() {
    MenuState state;
    state.onEnter();
    state.update(0.1f);

    sf::RenderTexture rt({DisplayConfig::LOGICAL_WIDTH, DisplayConfig::LOGICAL_HEIGHT});
    state.render(rt);
    rt.display();
}

void testLevel4SectionSnapshots() {
    Level level;
    level.setTheme(LevelTheme::CASTLE);
    const bool loaded = level.loadFromFile("levels/level4.txt");
    assert(loaded);

    level.update(0.016f);

    sf::RenderTexture rt({DisplayConfig::LOGICAL_WIDTH, DisplayConfig::LOGICAL_HEIGHT});

    // 1. Section 1 (Corridor, Firebars, Lava, Piranha Pipe at cols 31-32)
    level.getCamera().update(1.f, {750.f, 240.f});
    rt.clear(sf::Color::Black);
    level.render(rt);
    rt.display();

    // 2. Section 2 (Question Blocks, Removed Pit at cols 46-48)
    level.getCamera().update(1.f, {1500.f, 240.f});
    rt.clear(sf::Color::Black);
    level.render(rt);
    rt.display();

    // 3. Section 3 (Stepping platforms over lava)
    level.getCamera().update(1.f, {2800.f, 240.f});
    rt.clear(sf::Color::Black);
    level.render(rt);
    rt.display();

    // 4. Section 4 (Bowser Chamber: Springboard, Elevator, Cleared ceiling, Vine, Bowser, Axe, Bridge)
    level.getCamera().update(1.f, {4250.f, 240.f});
    rt.clear(sf::Color::Black);
    level.render(rt);
    rt.display();

    // 5. Section 4 with Elevator mid-flight over Bowser after 2 seconds
    for (int i = 0; i < 120; ++i) {
        level.update(0.016f);
    }
    level.getCamera().update(1.f, {4250.f, 240.f});
    rt.clear(sf::Color::Black);
    level.render(rt);
    rt.display();
}

void testBackgroundRendererLifecycle() {
    TextureManager& tm = TextureManager::getInstance();
    BackgroundRenderer bg;
    bg.init(tm, 3000.f, 480.f);
    assert(bg.getTheme() == LevelTheme::OVERWORLD);
    assert(close(bg.getScrollFactorX(), 0.35f));

    bg.setScrollFactorX(0.5f);
    assert(close(bg.getScrollFactorX(), 0.5f));
    bg.setScrollFactorX(0.35f);

    Camera cam;
    cam.init({640.f, 360.f}, sf::FloatRect({0.f, 0.f}, {3000.f, 480.f}));
    cam.update(0.f, {320.f, 240.f});

    sf::RenderTexture rt({640, 360});

    for (LevelTheme theme : {LevelTheme::OVERWORLD, LevelTheme::UNDERGROUND,
                             LevelTheme::UNDERWATER, LevelTheme::CASTLE}) {
        bg.setTheme(theme);
        assert(bg.getTheme() == theme);
        bg.update(0.05f);
        rt.clear(sf::Color::Black);
        bg.render(rt, cam);
        rt.display();
    }

    // Verify integration with Level
    Level level;
    level.setTheme(LevelTheme::UNDERGROUND);
    bool loaded = level.loadFromFile("levels/level2.txt");
    assert(loaded);
    assert(level.getBackgroundRenderer() != nullptr);
    assert(level.getBackgroundRenderer()->getTheme() == LevelTheme::UNDERGROUND);

    level.update(0.016f);
    rt.clear(sf::Color::Black);
    level.render(rt);
    rt.display();
}

// Regression lock for the campaign camera design decision (2026-08-23): the
// single-player camera follows Mario in BOTH directions — SMB1's never-
// scroll-back rule was intentionally dropped. Walking backward pans the view
// left so Mario stays inside the frame; the map's left boundary (Mario::
// applyWorldBoundsClamp, body center >= 16 px) remains the only hard limit.
void testCampaignBacktrackWithinMapBounds() {
    // Minimal deterministic fixture: flat 40x12 level, no pits, no enemies.
    const auto fixturePath =
        std::filesystem::temp_directory_path() / "mario_backtrack_level.txt";
    {
        std::ofstream out(fixturePath);
        assert(out.is_open());
        out << "# Temporary fixture: flat level for the backtrack test\n";
        for (int row = 0; row < 5; ++row) {
            out << std::string(40, '.') << '\n';
        }
        // Minimal valid flag ending in the far-right column (away from the
        // backtrack path): 'T' above 'F', pole '|' running down to the ground.
        out << std::string(39, '.') + 'T' << '\n';
        out << std::string(39, '.') + 'F' << '\n';
        for (int row = 0; row < 3; ++row) {
            out << std::string(39, '.') + '|' << '\n';
        }
        std::string spawnRow(39, '.');
        spawnRow += '|';
        spawnRow[33] = 'M';
        out << spawnRow << '\n';
        out << std::string(40, '0') << '\n';
    }

    Level level;
    assert(level.loadFromFile(fixturePath.string()));
    assert(!level.getCamera().isMonotonicScroll()); // two-way follow is on

    Mario* mario = level.getMario();
    assert(mario != nullptr);

    // Let the camera settle on the far-right spawn, then walk back left.
    for (int i = 0; i < 45; ++i) {
        level.update(1.f / 60.f);
    }
    const float centerBeforeWalk =
        level.getCamera().getView().getCenter().x;

    mario->setMoveIntent(-1.0f);
    bool stayedInFrame = true;
    for (int i = 0; i < 900; ++i) {
        level.update(1.f / 60.f);
        const sf::View& view = level.getCamera().getView();
        const float x = mario->getPosition().x;
        if (x + mario->getSize().x <
                view.getCenter().x - view.getSize().x / 2.f - 1.f ||
            x > view.getCenter().x + view.getSize().x / 2.f + 1.f) {
            stayedInFrame = false;
        }
        assert(view.getCenter().x <= centerBeforeWalk + 0.5f); // pans left
    }
    mario->stopMoving();
    for (int i = 0; i < 60; ++i) {
        level.update(1.f / 60.f);
    }

    // applyWorldBoundsClamp pins Mario's body CENTER at x = 16 px, so his
    // left edge rests around x = 2 px for the 28 px wide hitbox.
    const float restX = mario->getPosition().x;
    assert(!mario->isDying());
    assert(stayedInFrame);       // the view followed him the whole way back
    assert(level.getCamera().getView().getCenter().x <
           centerBeforeWalk - 100.f); // camera really traveled backward
    assert(restX >= 0.5f && restX <= 4.5f); // rests exactly at the map boundary
}

// Regression lock for the airborne-hold contract (2026-08-23 camera feel
// pass): while Mario is in the air, the campaign view applies NO vertical
// motion — jump arcs and falls never scroll the screen. The frame only
// re-anchors through the edge-margin rule after he lands somewhere new.
void testCampaignAirborneVerticalHold() {
    const auto fixturePath =
        std::filesystem::temp_directory_path() / "mario_airborne_hold_level.txt";
    {
        std::ofstream out(fixturePath);
        assert(out.is_open());
        out << "# Temporary fixture: flat level for the airborne hold test\n";
        for (int row = 0; row < 5; ++row) {
            out << std::string(40, '.') << '\n';
        }
        out << std::string(39, '.') + 'T' << '\n';
        out << std::string(39, '.') + 'F' << '\n';
        for (int row = 0; row < 3; ++row) {
            out << std::string(39, '.') + '|' << '\n';
        }
        std::string spawnRow(39, '.');
        spawnRow += '|';
        spawnRow[20] = 'M';
        out << spawnRow << '\n';
        out << std::string(40, '0') << '\n';
    }

    Level level;
    assert(level.loadFromFile(fixturePath.string()));

    Mario* mario = level.getMario();
    assert(mario != nullptr);

    for (int i = 0; i < 45; ++i) {
        level.update(1.f / 60.f);
    }

    // Launch Mario with jump-strength upward velocity mid-frame (as a real
    // jump would) — he is now airborne and rising.
    b2Body* body = mario->getBody();
    assert(body != nullptr);
    constexpr float JUMP_SPEED_PX = 460.f;
    body->SetLinearVelocity(
        b2Vec2(0.f, -PhysicsEngine::pixelsToMeters(JUMP_SPEED_PX)));

    const float centerBeforeJump =
        level.getCamera().getView().getCenter().y;

    bool wasAirborne = false;
    for (int i = 0; i < 30; ++i) {
        level.update(1.f / 60.f);
        if (!mario->isGrounded()) {
            wasAirborne = true;
        }
        // The view must not move vertically by even a fraction of a pixel
        // while the jump arc is in progress.
        assert(std::abs(level.getCamera().getView().getCenter().y -
                        centerBeforeJump) <= 0.001f);
    }
    assert(wasAirborne); // the fixture actually exercised the airborne state
}

// Regression: a missing or corrupt UI font used to leave m_menuTexts empty
// while processEvents() still indexed items [0..3] — out-of-bounds UB on the
// first mouse move or click while paused. The state must degrade gracefully
// instead (overlay + panel render, ESC resume still works via processInput).
void testPauseStateSurvivesMissingFont() {
    PauseState pause("assets/fonts/__missing_for_test__.ttf");
    assert(!pause.hasInteractiveMenu()); // degradation contract is active

    sf::RenderTexture rt({DisplayConfig::LOGICAL_WIDTH, DisplayConfig::LOGICAL_HEIGHT});
    rt.clear(sf::Color::Black);

    // Pre-fix these indexed an empty vector (out-of-bounds UB); the hardened
    // loops must process both events without touching menu items.
    pause.processEvents(sf::Event(sf::Event::MouseMoved{{10, 10}}));
    pause.processEvents(sf::Event(sf::Event::MouseButtonPressed{
        sf::Mouse::Button::Left, {10, 10}}));
    assert(!pause.hasInteractiveMenu());

    pause.update(0.016f);
    pause.render(rt);
    rt.display();
}

} // namespace

int main() {
    testIntegerViewportMatrix();
    testCameraPoliciesAndFourEdgeClamp();
    testCameraFollowsMarioAndReturnsToOriginalView();
    testMenuWidgetLogicalMouseClick();
    testLevelSelectStateRenderSnapshot();
    testMenuStateRenderSnapshot();
    testLevel4SectionSnapshots();
    testBackgroundRendererLifecycle();
    testCampaignBacktrackWithinMapBounds();
    testCampaignAirborneVerticalHold();
    testPauseStateSurvivesMissingFont();
    return 0;
}
