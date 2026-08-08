/**
 * @file SaveManager.h
 * @author TV4
 * @brief Persistent game data container and save-path management
 * @note Sprint 6 - version 1 save system foundation
 */

#pragma once

#include <string>

inline constexpr int SAVE_DATA_VERSION = 1;

struct SaveData {
    int version = SAVE_DATA_VERSION;
    int highScore = 0;
    int highestUnlockedLevel = 1;
    float soundVolume = 80.0f;
    float musicVolume = 70.0f;
};

class SaveManager {
public:
    // 1. Constructor / Destructor
    explicit SaveManager(const std::string& savePath = "saves/save.txt");
    ~SaveManager() = default;

    // 2. Public methods
    void resetToDefaults();

    // 3. Getters / Setters
    const SaveData& getData() const;
    const std::string& getSavePath() const;

private:
    // 4. Private members
    std::string m_savePath;
    SaveData m_data;
};
