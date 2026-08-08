/**
 * @file SaveManager.cpp
 * @author TV4
 * @brief Implementation of persistent game data and save-path management
 * @note Sprint 6 - version 1 save system foundation
 */

#include "core/SaveManager.h"

#include <fstream>
#include <iostream>
#include <string>

SaveManager::SaveManager(const std::string& savePath)
    : m_savePath(savePath),
      m_data{} {
}

bool SaveManager::load() {
    std::ifstream input(m_savePath);

    if (!input.is_open()) {
        resetToDefaults();
        return false;
    }

    SaveData loadedData{};
    std::string key;

    while (input >> key) {
        if (key == "version") {
            input >> loadedData.version;
        } else if (key == "highScore") {
            input >> loadedData.highScore;
        } else if (key == "highestUnlockedLevel") {
            input >> loadedData.highestUnlockedLevel;
        } else if (key == "soundVolume") {
            input >> loadedData.soundVolume;
        } else if (key == "musicVolume") {
            input >> loadedData.musicVolume;
        }
    }

    if (!input.eof() || loadedData.version != SAVE_DATA_VERSION) {
#ifdef DEBUG
        std::cerr << "[SaveManager] Failed to load valid save data." << std::endl;
#endif
        resetToDefaults();
        return false;
    }

    m_data = loadedData;
    return true;
}

void SaveManager::resetToDefaults() {
    m_data = SaveData{};
}

const SaveData& SaveManager::getData() const {
    return m_data;
}

const std::string& SaveManager::getSavePath() const {
    return m_savePath;
}
