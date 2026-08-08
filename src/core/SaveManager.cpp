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
#include <algorithm>
#include <cmath>

namespace {

constexpr float MIN_VOLUME = 0.0f;
constexpr float MAX_VOLUME = 100.0f;

float clampVolume(float volume) {
    if (!std::isfinite(volume)) {
        return MIN_VOLUME;
    }

    return std::clamp(volume, MIN_VOLUME, MAX_VOLUME);
}

}

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

bool SaveManager::save() const {
    std::ofstream output(m_savePath, std::ios::trunc);

    if (!output.is_open()) {
#ifdef DEBUG
        std::cerr << "[SaveManager] Failed to open save file for writing." << std::endl;
#endif
        return false;
    }

    output << "version " << m_data.version << std::endl;
    output << "highScore " << m_data.highScore << std::endl;
    output << "highestUnlockedLevel " << m_data.highestUnlockedLevel << std::endl;
    output << "soundVolume " << m_data.soundVolume << std::endl;
    output << "musicVolume " << m_data.musicVolume << std::endl;

    return output.good();
}

void SaveManager::resetToDefaults() {
    m_data = SaveData{};
}

bool SaveManager::updateAudioSettings(float soundVolume, float musicVolume) {
    const float previousSoundVolume = m_data.soundVolume;
    const float previousMusicVolume = m_data.musicVolume;

    m_data.soundVolume = clampVolume(soundVolume);
    m_data.musicVolume = clampVolume(musicVolume);

    if (!save()) {
        m_data.soundVolume = previousSoundVolume;
        m_data.musicVolume = previousMusicVolume;

        return false;
    }

    return true;
}

bool SaveManager::updateHighScore(int score) {
    if (score <= m_data.highScore) {
        return false;
    }

    const int previousHighScore = m_data.highScore;

    m_data.highScore = score;

    if (!save()) {
        m_data.highScore = previousHighScore;
        return false;
    }

    return true;
}

bool SaveManager::updateHighestUnlockedLevel(int level) {
    if (level <= m_data.highestUnlockedLevel) {
        return false;
    }

    const int previousHighestUnlockedLevel = m_data.highestUnlockedLevel;

    m_data.highestUnlockedLevel = level;

    if (!save()) {
        m_data.highestUnlockedLevel = previousHighestUnlockedLevel;
        return false;
    }

    return true;
}

const SaveData& SaveManager::getData() const {
    return m_data;
}

const std::string& SaveManager::getSavePath() const {
    return m_savePath;
}
