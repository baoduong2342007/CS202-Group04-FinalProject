/**
 * @file GameProgress.h
 * @author TV1 (Duong)
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
    CharacterType character = CharacterType::MARIO;
    // Co-op extension: score/coins/lives stay the shared team totals while
    // character2 names player two's fighter. Defaults keep the single-player
    // contract unchanged.
    bool isCoop = false;
    CharacterType character2 = CharacterType::LUIGI;
};
