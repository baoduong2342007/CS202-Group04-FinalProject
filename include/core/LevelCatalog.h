/**
 * @file LevelCatalog.h
 * @author TV1 (Dương)
 * @brief Centralized level catalog — Sprint 6 (S6-TV1-05)
 * @note Sprint 6 locked contract — do not change without TV1 approval
 */

#pragma once

#include <string>
#include <vector>

enum class LevelTheme {
    OVERWORLD,
    UNDERGROUND,
    CASTLE
};

enum class MusicId {
    OVERWORLD,
    UNDERGROUND,
    CASTLE,
    STAR,
    DEATH,
    GAME_OVER,
    WIN
};

// Sprint 6 locked contract (S6-TV1-03)
struct LevelDefinition {
    int number;                 // one-based (1, 2, 3)
    std::string filePath;       // e.g. "levels/level1.txt"
    std::string worldLabel;     // e.g. "1-1"
    LevelTheme theme;
    MusicId music;
};

namespace LevelCatalog {
    /// Returns the full list of release levels (1, 2, 3).
    inline const std::vector<LevelDefinition>& getAll() {
        static const std::vector<LevelDefinition> catalog = {
            {1, "levels/level1.txt", "1-1", LevelTheme::OVERWORLD,   MusicId::OVERWORLD},
            {2, "levels/level2.txt", "1-2", LevelTheme::UNDERGROUND, MusicId::UNDERGROUND},
            {3, "levels/level3.txt", "1-3", LevelTheme::CASTLE,      MusicId::CASTLE},
        };
        return catalog;
    }

    /// Find a LevelDefinition by one-based number. Returns nullptr if not found.
    inline const LevelDefinition* find(int levelNumber) {
        for (const auto& def : getAll()) {
            if (def.number == levelNumber) return &def;
        }
        return nullptr;
    }

    /// Total number of release levels.
    inline int count() {
        return static_cast<int>(getAll().size());
    }
}