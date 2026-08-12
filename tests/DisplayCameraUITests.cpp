/**
 * @file DisplayCameraUITests.cpp
 * @author TV2
 * @brief Integer viewport, mouse remap, camera policy and UI mouse regression.
 */

#include <cassert>
#include <cmath>

#include "core/DisplayConfig.h"
#include "level/Camera.h"
#include "ui/UIMenuWidget.h"

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
    assert(close(tall.getView().getCenter().y, 180.f));
    assertViewInside(tall, tallBounds);
    tall.update(0.f, {500.f, 1150.f});
    assert(close(tall.getView().getCenter().y, 1020.f));
    assertViewInside(tall, tallBounds);

    Camera small;
    small.init({640.f, 360.f}, {{200.f, 100.f}, {300.f, 200.f}});
    small.update(0.f, {-1000.f, -1000.f});
    assert(small.getView().getCenter() == sf::Vector2f(350.f, 200.f));
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
} // namespace

int main() {
    testIntegerViewportMatrix();
    testCameraPoliciesAndFourEdgeClamp();
    testMenuWidgetLogicalMouseClick();
    return 0;
}
