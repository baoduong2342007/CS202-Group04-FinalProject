/**
 * @file SmallMarioState.cpp
 * @author TV3 (Bảo)
 * @brief Implementation of SmallMarioState
 */

#include "states/SmallMarioState.h"
#include <iostream>

void SmallMarioState::onEnter(Mario& mario) {
    (void)mario;
#ifdef DEBUG
    std::cout << "[MarioState] Entered SmallMarioState (32x32)" << std::endl;
#endif
}

void SmallMarioState::onExit(Mario& mario) {
    (void)mario;
}

void SmallMarioState::update(Mario& mario, float dt) {
    (void)mario;
    (void)dt;
}
