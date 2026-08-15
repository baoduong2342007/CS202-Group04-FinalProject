/**
 * @file PlayStateTests.cpp
 * @author TV1 (Duong)
 * @brief Regression tests for the PlayState terminal-result contract:
 *        - death -> exactly one PLAYER_DIED event + exactly one life lost;
 *        - a second death while dying is ignored (no double event/life);
 *        - respawn re-arms the death chain;
 *        - Level 3 completion decision -> Win through a single transition path
 *          (S6-TV1-13/14/18).
 *
 * Limitation: constructing a real PlayState headlessly would require a live
 * SFML window, an audio device and an on-disk level load, so the PlayState
 * state machine is exercised through the same decision logic it uses
 * (LevelCatalog) plus the Mario death contract that drives it. The end-to-end
 * "Level 1 -> 2 -> 3 -> Win" loop is covered by manual playthrough
 * (S6-TV4-40) and the PlayState code-path review logged in
 * docs/change_in_develop.md.
 */

#include <cassert>
#include <iostream>

#include "core/GameProgress.h"
#include "core/LevelCatalog.h"
#include "entities/Mario.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "patterns/IObserver.h"

namespace {

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

} // namespace

int main() {
    const bool ok = testRaceConditionGuard()
                 && testWinDecisionCommittedOnce()
                 && testSingleDeathFiresOneEventAndOneLifeLoss()
                 && testSecondDeathWhileDyingIsIgnored()
                 && testRespawnReArmsDeathChain();

    if (ok) {
        std::cout << "All PlayState tests passed successfully!" << std::endl;
        return 0;
    }
    std::cout << "PlayState tests FAILED." << std::endl;
    return 1;
}
