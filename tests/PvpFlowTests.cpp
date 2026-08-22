/**
 * @file PvpFlowTests.cpp
 * @author TV5 (Truyen)
 * @brief PvP round state machine, fire-flower director and character-select
 *        phase tests, driven against the shipped arena.
 */

#include <cassert>
#include <iostream>

#include <SFML/Window/Keyboard.hpp>

#include "entities/Mario.h"
#include "level/Level.h"
#include "level/TileMap.h"
#include "patterns/InputState.h"
#include "states/PvpCharacterSelectState.h"
#include "states/PvpPlayState.h"

namespace {

constexpr float FRAME_DT = 1.f / 60.f;

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

InputState idleInput() {
    InputState state;
    state.beginFrame();
    return state;
}

/// Drive updates until `predicate` holds or `maxFrames*FRAME_DT` elapse.
template <typename Predicate>
bool waitFor(PvpPlayState& state, Predicate predicate, int maxFrames) {
    for (int frame = 0; frame < maxFrames; ++frame) {
        if (predicate()) {
            return true;
        }
        state.update(FRAME_DT);
    }
    return predicate();
}

/// Teleport player one squarely above player two and let him drop onto the
/// opponent's head. Returns true when the duel left the FIGHT phase.
bool dropPlayerOneOntoPlayerTwo(PvpPlayState& state) {
    Level* level = state.getLevel();
    Mario* p1 = level->getMario();
    Mario* p2 = level->getMario2();

    p2->setPosition({300.f, 252.f});
    p2->setVelocity({0.f, 0.f});
    p1->setPosition({300.f, 214.f});
    p1->setVelocity({0.f, 200.f});

    return waitFor(
        state,
        [&state] { return state.getPhase() != PvpPlayState::Phase::FIGHT; },
        180);
}

bool testRoundIntroThenFight() {
    std::cout << "[RUNNING] testRoundIntroThenFight..." << std::endl;

    PvpPlayState state(CharacterType::MARIO, CharacterType::LUIGI);
    state.onEnter();

    assert(state.getLevel() != nullptr);
    assert(state.getPhase() == PvpPlayState::Phase::ROUND_INTRO);
    assert(state.getRoundNumber() == 1);
    assert(state.getRoundWins(0) == 0 && state.getRoundWins(1) == 0);

    // Input is frozen during the intro: fighters keep their zero intents.
    state.processInput(idleInput());
    assert(state.getLevel()->getMario()->getHorizontalIntent() == 0.f);

    assert(waitFor(state,
                   [&state] {
                       return state.getPhase() == PvpPlayState::Phase::FIGHT;
                   },
                   200));
    assert(!state.isFlowerOnField());
    assert(state.getMatchWinner() == -1);

    std::cout << "[PASSED] testRoundIntroThenFight" << std::endl;
    return true;
}

bool testStompWinsRoundAndMatch() {
    std::cout << "[RUNNING] testStompWinsRoundAndMatch..." << std::endl;

    PvpPlayState state(CharacterType::MARIO, CharacterType::LUIGI);
    state.onEnter();
    waitFor(state,
            [&state] { return state.getPhase() == PvpPlayState::Phase::FIGHT; },
            200);

    // Round 1: P1 stomps P2.
    assert(dropPlayerOneOntoPlayerTwo(state));
    assert(state.getRoundWins(0) == 1);
    assert(state.getRoundWins(1) == 0);
    assert(state.getLevel()->getMario2()->isDying());

    // The loser respawns and round 2 starts with a clean slate.
    assert(waitFor(state,
                   [&state] {
                       return state.getPhase() == PvpPlayState::Phase::FIGHT &&
                              state.getRoundNumber() == 2;
                   },
                   600));
    assert(!state.getLevel()->getMario2()->isDying());
    assert(!state.isFlowerOnField());

    // Round 2: P1 takes the match (first to two round wins).
    assert(dropPlayerOneOntoPlayerTwo(state));
    assert(waitFor(state,
                   [&state] {
                       return state.getPhase() == PvpPlayState::Phase::MATCH_END;
                   },
                   600));
    assert(state.getMatchWinner() == 0);
    assert(state.getRoundWins(0) == 2);

    std::cout << "[PASSED] testStompWinsRoundAndMatch" << std::endl;
    return true;
}

bool testRematchResetsMatch() {
    std::cout << "[RUNNING] testRematchResetsMatch..." << std::endl;

    PvpPlayState state(CharacterType::MARIO, CharacterType::LUIGI);
    state.onEnter();
    waitFor(state,
            [&state] { return state.getPhase() == PvpPlayState::Phase::FIGHT; },
            200);
    assert(dropPlayerOneOntoPlayerTwo(state));
    waitFor(state,
            [&state] { return state.getPhase() == PvpPlayState::Phase::FIGHT; },
            600);
    assert(dropPlayerOneOntoPlayerTwo(state));
    waitFor(state,
                   [&state] {
                       return state.getPhase() == PvpPlayState::Phase::MATCH_END;
                   },
                   600);
    assert(state.getMatchWinner() == 0);

    // ENTER on the match screen starts a fresh match in place.
    const PressedKey enter(sf::Keyboard::Key::Enter);
    state.processInput(enter.state());

    assert(state.getPhase() == PvpPlayState::Phase::ROUND_INTRO);
    assert(state.getRoundNumber() == 1);
    assert(state.getRoundWins(0) == 0 && state.getRoundWins(1) == 0);
    assert(state.getMatchWinner() == -1);
    assert(!state.getLevel()->getMario()->isDying());
    assert(!state.getLevel()->getMario2()->isDying());

    std::cout << "[PASSED] testRematchResetsMatch" << std::endl;
    return true;
}

bool testFireFlowerDirectorAndTimedFire() {
    std::cout << "[RUNNING] testFireFlowerDirectorAndTimedFire..." << std::endl;

    PvpPlayState state(CharacterType::MARIO, CharacterType::LUIGI);
    state.onEnter();
    waitFor(state,
            [&state] { return state.getPhase() == PvpPlayState::Phase::FIGHT; },
            200);

    // Inject a short countdown; the flower must appear on the pedestal.
    state.setNextFlowerCountdown(0.5f);
    assert(waitFor(state,
                   [&state] { return state.isFlowerOnField(); },
                   120));
    Level* level = state.getLevel();

    // While a flower is on the field no second one may spawn, even with a
    // zeroed countdown.
    state.setNextFlowerCountdown(0.f);
    for (int frame = 0; frame < 10; ++frame) {
        state.update(FRAME_DT);
    }
    assert(state.isFlowerOnField());

    // Player one grabs the flower: FIRE state with a 5-second budget.
    Mario* p1 = level->getMario();
    const sf::Vector2i pedestal =
        level->getTileMap().findTiles('W').front();
    p1->setPosition(TileMap::gridToWorldPosition(pedestal));
    p1->setVelocity({0.f, 0.f});

    assert(waitFor(state,
                   [&p1] {
                       return p1->getMarioState() == MarioState::FIRE_SMALL;
                   },
                   120));
    // The pickup edge arms the 5s budget on the update AFTER the collect
    // frame, so advance one more frame before reading the timer.
    state.update(FRAME_DT);
    assert(state.getFireTimer(0) > 4.5f);
    assert(!state.isFlowerOnField());

    // No replacement flower while the fire state is live.
    state.setNextFlowerCountdown(0.f);
    for (int frame = 0; frame < 10; ++frame) {
        state.update(FRAME_DT);
    }
    assert(!state.isFlowerOnField());

    // After five seconds the fighter reverts to SMALL.
    assert(waitFor(state,
                   [&p1] { return p1->getMarioState() == MarioState::SMALL; },
                   400));
    assert(state.getFireTimer(0) == 0.f);

    // Once the fire expired the director can arm the next countdown again.
    state.setNextFlowerCountdown(0.1f);
    assert(waitFor(state,
                   [&state] { return state.isFlowerOnField(); },
                   60));

    std::cout << "[PASSED] testFireFlowerDirectorAndTimedFire" << std::endl;
    return true;
}

bool testCharacterSelectSequentialPhases() {
    std::cout << "[RUNNING] testCharacterSelectSequentialPhases..." << std::endl;

    PvpCharacterSelectState select;
    select.onEnter();
    assert(select.getCurrentPicker() == 0);

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

    std::cout << "[PASSED] testCharacterSelectSequentialPhases" << std::endl;
    return true;
}

} // namespace

int main() {
    const bool success = testRoundIntroThenFight() &&
                         testStompWinsRoundAndMatch() &&
                         testRematchResetsMatch() &&
                         testFireFlowerDirectorAndTimedFire() &&
                         testCharacterSelectSequentialPhases();

    if (success) {
        std::cout << "All PvpFlow tests passed successfully!" << std::endl;
        return 0;
    }

    std::cout << "PvpFlow tests FAILED." << std::endl;
    return 1;
}
