/**
 * @file FireMarioState.cpp
 * @author TV3 (Bảo)
 * @brief Implementation of FireMarioState
 */

#include "states/FireMarioState.h"
#include <iostream>

void FireMarioState::onEnter(Mario& mario) {
    (void)mario;
#ifdef DEBUG
    std::cout << "[MarioState] Entered FireMarioState (32x64, Fireball Enabled)" << std::endl;
#endif
}

void FireMarioState::onExit(Mario& mario) {
    (void)mario;
}

void FireMarioState::update(Mario& mario, float dt) {
    (void)mario;
    (void)dt;
}
