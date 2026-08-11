/**
 * @file SaveManager.cpp
 * @author TV4
 * @brief Implementation of persistent game data and save-path management
 * @note Sprint 6 - version 1 save system foundation
 */

#include "core/SaveManager.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <system_error>
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

constexpr const char* TEMP_FILE_SUFFIX = ".tmp";

bool readExpectedKey(std::istream& input, const char* expectedKey) {
    std::string key;

    if (!(input >> key)) {
        return false;
    }

    return key == expectedKey;
}

bool parseSaveData(std::istream& input, SaveData& data) {
    if (!readExpectedKey(input, "version") || !(input >> data.version)) {
        return false;
    }

    if (!readExpectedKey(input, "highScore") || !(input >> data.highScore)) {
        return false;
    }

    if (!readExpectedKey(input, "highestUnlockedLevel") || !(input >> data.highestUnlockedLevel)) {
        return false;
    }

    if (!readExpectedKey(input, "soundVolume") || !(input >> data.soundVolume)) {
        return false;
    }

    if (!readExpectedKey(input, "musicVolume") || !(input >> data.musicVolume)) {
        return false;
    }

    std::string extraToken;

    if (input >> extraToken) {
        return false;
    }

    return input.eof();
}

bool hasValidSaveValues(const SaveData& data) {
    const bool soundVolumeValid = std::isfinite(data.soundVolume) &&
                                  data.soundVolume >= MIN_VOLUME &&
                                  data.soundVolume <= MAX_VOLUME;

    const bool musicVolumeValid = std::isfinite(data.musicVolume) &&
                                  data.musicVolume >= MIN_VOLUME &&
                                  data.musicVolume <= MAX_VOLUME;

    return data.highScore >= 0 && data.highestUnlockedLevel >= 1 &&
           soundVolumeValid && musicVolumeValid;
}

void logSaveFallback(const char* reason) {
    std::cerr << "[SaveManager] " << reason << " Using default save data." << std::endl;
}

} // namespace

SaveManager::SaveManager(const std::string& savePath)
    : m_savePath(savePath),
      m_data{} {
}

bool SaveManager::load() {
    std::ifstream input(m_savePath);

    if (!input.is_open()) {
        resetToDefaults();
        logSaveFallback("Save file was not found.");
        return false;
    }

    SaveData loadedData{};

    if (!parseSaveData(input, loadedData)) {
        resetToDefaults();
        logSaveFallback("Save file is corrupted.");
        return false;
    }

    if (loadedData.version != SAVE_DATA_VERSION) {
        resetToDefaults();
        logSaveFallback("Save file version is not supported.");
        return false;
    }

    if (!hasValidSaveValues(loadedData)) {
        resetToDefaults();
        logSaveFallback("Save file contains invalid values.");
        return false;
    }

    m_data = loadedData;
    return true;
}

bool SaveManager::save() const {
    const std::string temporaryPath = m_savePath + TEMP_FILE_SUFFIX;

    if (!writeSaveFile(temporaryPath)) {
        std::error_code errorCode;
        std::filesystem::remove(temporaryPath, errorCode);

        return false;
    }

    if (!replaceSaveFile(temporaryPath)) {
        std::error_code errorCode;
        std::filesystem::remove(temporaryPath, errorCode);

        return false;
    }

    return true;
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

bool SaveManager::replaceSaveFile(const std::string& temporaryPath) const {
    std::error_code errorCode;

    std::filesystem::rename(temporaryPath, m_savePath, errorCode);

    if (errorCode) {
        std::cerr << "[SaveManager] Failed to replace save file at '"
                  << m_savePath << "' (rename error: " << errorCode.message() << ")."
                  << std::endl;
        return false;
    }

    return true;
}

bool SaveManager::writeSaveFile(const std::string& path) const {
    const std::filesystem::path savePath(path);

    const std::filesystem::path parentPath = savePath.parent_path();

    if (!parentPath.empty()) {
        std::error_code errorCode;

        std::filesystem::create_directories(parentPath, errorCode);

        if (errorCode) {
            std::cerr << "[SaveManager] Failed to create save directory '"
                      << parentPath.string() << "' (error: " << errorCode.message() << ")."
                      << std::endl;
            return false;
        }
    }

    std::ofstream output(path, std::ios::trunc);

    if (!output.is_open()) {
        std::cerr << "[SaveManager] Failed to open temporary save file '"
                  << path << "'." << std::endl;
        return false;
    }

    output << "version " << m_data.version << std::endl;
    output << "highScore " << m_data.highScore << std::endl;
    output << "highestUnlockedLevel " << m_data.highestUnlockedLevel << std::endl;
    output << "soundVolume " << m_data.soundVolume << std::endl;
    output << "musicVolume " << m_data.musicVolume << std::endl;

    output.flush();

    const bool writeSucceeded = output.good();
    output.close();

    return writeSucceeded;
}
