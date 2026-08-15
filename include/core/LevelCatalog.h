/**
 * @file LevelCatalog.h
 * @author TV1 (Dương)
 * @brief Centralized level catalog — Sprint 7 (S7-TV1-02)
 * @note Sprint 7 locked contract — Level 1 -> 2 -> 3 -> 4 -> Win
 */

#pragma once

#include <string>
#include <vector>

enum class LevelTheme {
    OVERWORLD,
    UNDERGROUND,
    CASTLE,
    UNDERWATER
};

enum class MusicId {
    OVERWORLD,
    UNDERGROUND,
    CASTLE,
    UNDERWATER,
    STAR,
    DEATH,
    GAME_OVER,
    WIN
};

enum class CameraVerticalMode {
    LOCKED,
    DEAD_ZONE
};

// Sprint 7 locked contract (S7-TV1-02)
struct LevelDefinition {
    int number;                 // one-based release level number
    std::string filePath;       // e.g. "levels/level1.txt"
    std::string worldLabel;     // e.g. "1-1"
    LevelTheme theme;
    MusicId music;
    CameraVerticalMode cameraMode;
};

namespace LevelCatalog {
    /// Returns the locked Sprint 7 release graph: Level 1 -> 2 -> 3 -> 4 -> Win.
    inline const std::vector<LevelDefinition>& getAll() {
        static const std::vector<LevelDefinition> catalog = {
            {1, "levels/level1.txt", "1-1", LevelTheme::OVERWORLD,
                MusicId::OVERWORLD, CameraVerticalMode::DEAD_ZONE},
            {2, "levels/level2.txt", "1-2", LevelTheme::UNDERGROUND,
                MusicId::UNDERGROUND, CameraVerticalMode::DEAD_ZONE},
            {3, "levels/level3.txt", "1-3", LevelTheme::UNDERWATER,
                MusicId::UNDERWATER, CameraVerticalMode::DEAD_ZONE},
            {4, "levels/level4.txt", "1-4", LevelTheme::CASTLE,
                MusicId::CASTLE, CameraVerticalMode::DEAD_ZONE},
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

    /// Checks whether the given level number exceeds the highest configured release level.
    inline bool isPastFinalLevel(int levelNumber) {
        const auto& catalog = getAll();
        if (catalog.empty()) return true;
        return levelNumber > catalog.back().number;
    }
}
