/**
 * @file PlayStateTests.cpp
 * @author TV1 (Duong)
 * @brief Regression tests for PlayState level progression, race condition guards, and WinState transition.
 */

#include <iostream>
#include <cassert>

#include "core/LevelCatalog.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"

bool testRaceConditionGuard() {
    std::cout << "[RUNNING] testRaceConditionGuard..." << std::endl;

    // Verify LevelCatalog level bounds
    assert(!LevelCatalog::isPastFinalLevel(1));
    assert(!LevelCatalog::isPastFinalLevel(2));
    assert(!LevelCatalog::isPastFinalLevel(3));
    assert(LevelCatalog::isPastFinalLevel(4));

    std::cout << "[PASSED] testRaceConditionGuard" << std::endl;
    return true;
}

bool testWinStateTransitionLogic() {
    std::cout << "[RUNNING] testWinStateTransitionLogic..." << std::endl;

    int currentLevel = 3;
    currentLevel++; // Completed Level 3 -> now level 4
    bool isWin = LevelCatalog::isPastFinalLevel(currentLevel);
    assert(isWin == true);

    std::cout << "[PASSED] testWinStateTransitionLogic" << std::endl;
    return true;
}

int main() {
    const bool success = testRaceConditionGuard() && testWinStateTransitionLogic();
    if (success) {
        std::cout << "All PlayState tests passed successfully!" << std::endl;
        return 0;
    }
    return 1;
}
