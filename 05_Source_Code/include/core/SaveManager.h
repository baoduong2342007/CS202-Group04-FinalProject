/**
 * @file SaveManager.h
 * @author TV4
 * @brief Persistent game data container and save-path management
 * @note Sprint 6 - version 1 save system foundation
 */

#pragma once

#include <array>
#include <string>
#include <vector>

inline constexpr int SAVE_DATA_VERSION = 1;

struct GameRecord {
    std::string date;               ///< e.g. "2026-09-02 16:30"
    int level = 1;                  ///< 1, 2, 3, 4
    std::string mode = "SOLO";      ///< "SOLO", "CO-OP", "PVP"
    std::string character = "Mario";///< "Mario", "Luigi", "Co-op"
    std::string result = "CLEARED"; ///< "CLEARED", "GAME OVER", "P1 WIN", "P2 WIN"
    int score = 0;
    int coins = 0;
};

struct SaveData {
    int version = SAVE_DATA_VERSION;
    int highScore = 0;
    std::array<int, 4> levelHighScores = {0, 0, 0, 0};
    int highestUnlockedLevel = 1;
    float soundVolume = 80.0f;
    float musicVolume = 70.0f;
    std::vector<GameRecord> matchHistory;
};

class SaveManager {
public:
    // 1. Constructor / Destructor
    explicit SaveManager(const std::string& savePath = "saves/save.txt");
    ~SaveManager() = default;

    // 2. Public methods
    bool load();
    bool save() const;
    void resetToDefaults();
    bool updateAudioSettings(float soundVolume, float musicVolume);
    bool updateHighScore(int globalScore, int stageScore, int level = 0);
    bool updateHighestUnlockedLevel(int level);
    bool addGameRecord(const GameRecord& record);
    bool deleteGameRecord(std::size_t index);
    bool clearMatchHistory();
    bool resetAllData();

    // 3. Getters / Setters
    int getHighScore(int level = 0) const;
    int getLevelHighScore(int level) const;
    const std::vector<GameRecord>& getMatchHistory() const;
    const SaveData& getData() const;
    const std::string& getSavePath() const;

private:
    // 4. Private members
    bool replaceSaveFile(const std::string& temporaryPath) const;
    bool writeSaveFile(const std::string& path) const;

    // 5. Private members
    std::string m_savePath;
    SaveData m_data;
};
