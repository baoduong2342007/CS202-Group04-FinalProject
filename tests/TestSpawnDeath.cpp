#include "states/PlayState.h"
#include <iostream>

int main() {
    std::cout << "[TEST] Starting PlayState diagnostic test..." << std::endl;
    PlayState playState;
    playState.onEnter();

    for (int frame = 0; frame < 60; ++frame) {
        std::cout << "--- Frame " << frame << " ---" << std::endl;
        playState.update(0.016f);
    }
    std::cout << "[TEST] PlayState diagnostic test complete." << std::endl;
    return 0;
}
