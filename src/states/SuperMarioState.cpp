/**
 * @file SuperMarioState.cpp
 * @author TV3 (Bảo)
 * @brief Implementation of SuperMarioState
 */

#include "states/SuperMarioState.h"
#include <iostream>

void SuperMarioState::onEnter(Mario& mario) {
    (void)mario;
#ifdef DEBUG
    std::cout << "[MarioState] Entered SuperMarioState (32x64)" << std::endl;
#endif
}

void SuperMarioState::onExit(Mario& mario) {
    (void)mario;
}

void SuperMarioState::update(Mario& mario, float dt) {
    (void)mario;
    (void)dt;
}
