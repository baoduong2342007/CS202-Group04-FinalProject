/**
 * @file GameProgress.h
 * @author TV1 (Dương)
 * @brief Session progress data, independent of Level lifetime
 * @note Sprint 6 — S6-TV1-08: progress lives in the game session (PlayState),
 *       NOT inside Level, so destroying a Level never resets score/coins/lives.
 */

#pragma once

#include "entities/Mario.h"

// ============================================================
// Sprint 6 locked contract (S6-TV1-03) — do not change without TV1 approval
// ============================================================
struct GameProgress {
    int currentLevel = 1;   // one-based level number (World 1-1 -> 1, 1-2 -> 2, 1-3 -> 3)
    int score = 0;
    int coins = 0;
    int lives = 3;
    MarioState power = MarioState::SMALL;
};