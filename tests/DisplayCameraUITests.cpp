/**
 * @file DisplayCameraUITests.cpp
 * @author TV2
 * @brief Integer viewport, mouse remap, camera policy and UI mouse regression.
 */

#include <cassert>
#include <cmath>

#include "core/DisplayConfig.h"
#include "core/SpriteFrames_ovw.h"
#include "core/SpriteFrames_udg.h"
#include "core/SpriteFrames_udw.h"
#include "core/SpriteFrames_castle.h"
#include "core/SpriteFrames_shared.h"
#include "level/TileFrames.h"
#include "level/Camera.h"
#include "level/TileMap.h"
#include "ui/UIMenuWidget.h"
#include "states/LevelSelectState.h"

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

    const bool saved = rt.getTexture().copyToImage().saveToFile("level_select_preview.png");
    assert(saved);
}
} // namespace

int main() {
    testIntegerViewportMatrix();
    testCameraPoliciesAndFourEdgeClamp();
    testCameraFollowsMarioAndReturnsToOriginalView();
    testMenuWidgetLogicalMouseClick();
    testLevelSelectStateRenderSnapshot();
    return 0;
}
