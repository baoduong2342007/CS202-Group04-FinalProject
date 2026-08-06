/**
 * @file InputStateTests.cpp
 * @author TV5 (Truyen)
 * @brief Regression tests for event-driven command input dispatch.
 */

#include <cstdlib>
#include <iostream>
#include <memory>

#include "patterns/ICommand.h"
#include "patterns/InputHandler.h"
#include "patterns/InputState.h"

namespace {

class RecordingCommand final : public ICommand {
public:
    RecordingCommand(int& executionCount, int& lastValue, int value)
        : m_executionCount(executionCount), m_lastValue(lastValue), m_value(value) {}

    void execute() override {
        ++m_executionCount;
        m_lastValue = m_value;
    }

private:
    int& m_executionCount;
    int& m_lastValue;
    int m_value;
};

sf::Event keyPressed(sf::Keyboard::Key key) {
    return sf::Event(sf::Event::KeyPressed{key, sf::Keyboard::Scan::Unknown, false, false, false, false});
}

sf::Event keyReleased(sf::Keyboard::Key key) {
    return sf::Event(sf::Event::KeyReleased{key, sf::Keyboard::Scan::Unknown, false, false, false, false});
}

bool check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return false;
    }
    return true;
}

bool testShortTapDispatchesOnce() {
    InputState input;
    InputHandler handler;
    int executions = 0;
    int direction = 0;

    handler.bindKey(sf::Keyboard::Key::Right,
                    std::make_unique<RecordingCommand>(executions, direction, 1),
                    InputTrigger::Held,
                    InputGroup::Horizontal);

    input.beginFrame();
    input.handleEvent(keyPressed(sf::Keyboard::Key::Right));
    input.handleEvent(keyReleased(sf::Keyboard::Key::Right));
    handler.handleInput(input);

    return check(!input.isHeld(sf::Keyboard::Key::Right), "short tap must finish released") &&
           check(input.wasPressed(sf::Keyboard::Key::Right), "short tap must retain press edge") &&
           check(executions == 1 && direction == 1, "short tap must dispatch the right command once");
}

bool testHeldKeyAndAutoRepeat() {
    InputState input;
    InputHandler handler;
    int executions = 0;
    int direction = 0;

    handler.bindKey(sf::Keyboard::Key::Right,
                    std::make_unique<RecordingCommand>(executions, direction, 1),
                    InputTrigger::Held,
                    InputGroup::Horizontal);

    input.beginFrame();
    input.handleEvent(keyPressed(sf::Keyboard::Key::Right));
    handler.handleInput(input);
    input.beginFrame();
    input.handleEvent(keyPressed(sf::Keyboard::Key::Right));
    handler.handleInput(input);

    return check(executions == 2, "held key must dispatch once every frame") &&
           check(!input.wasPressed(sf::Keyboard::Key::Right), "key auto-repeat must not create a new press edge") &&
           check(direction == 1, "held key must keep its command active");
}

bool testLatestDirectionWinsAndFallsBack() {
    InputState input;
    InputHandler handler;
    int executions = 0;
    int direction = 0;

    handler.bindKey(sf::Keyboard::Key::Left,
                    std::make_unique<RecordingCommand>(executions, direction, -1),
                    InputTrigger::Held,
                    InputGroup::Horizontal);
    handler.bindKey(sf::Keyboard::Key::Right,
                    std::make_unique<RecordingCommand>(executions, direction, 1),
                    InputTrigger::Held,
                    InputGroup::Horizontal);

    input.beginFrame();
    input.handleEvent(keyPressed(sf::Keyboard::Key::Left));
    handler.handleInput(input);
    if (!check(direction == -1, "left must work when it is the only held direction")) return false;

    input.beginFrame();
    input.handleEvent(keyPressed(sf::Keyboard::Key::Right));
    handler.handleInput(input);
    if (!check(direction == 1, "most recently pressed direction must win")) return false;

    input.beginFrame();
    input.handleEvent(keyReleased(sf::Keyboard::Key::Right));
    handler.handleInput(input);
    return check(direction == -1, "releasing the winning direction must restore the held direction") &&
           check(executions == 3, "only one horizontal command may execute per frame");
}

bool testPressedCommandAndFocusLoss() {
    InputState input;
    InputHandler handler;
    int executions = 0;
    int value = 0;

    handler.bindKey(sf::Keyboard::Key::Space,
                    std::make_unique<RecordingCommand>(executions, value, 1),
                    InputTrigger::Pressed);

    input.beginFrame();
    input.handleEvent(keyPressed(sf::Keyboard::Key::Space));
    handler.handleInput(input);
    input.beginFrame();
    input.handleEvent(keyPressed(sf::Keyboard::Key::Space));
    handler.handleInput(input);
    if (!check(executions == 1, "pressed command must ignore auto-repeat")) return false;

    input.handleEvent(sf::Event(sf::Event::FocusLost{}));
    return check(!input.isHeld(sf::Keyboard::Key::Space), "focus loss must clear held keys") &&
           check(!input.wasPressed(sf::Keyboard::Key::Space), "focus loss must clear press edges");
}

} // namespace

int main() {
    const bool success = testShortTapDispatchesOnce() &&
                         testHeldKeyAndAutoRepeat() &&
                         testLatestDirectionWinsAndFallsBack() &&
                         testPressedCommandAndFocusLoss();
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
