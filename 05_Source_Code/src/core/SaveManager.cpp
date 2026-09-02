/**
 * @file SaveManager.cpp
 * @author TV4
 * @brief Implementation of persistent game data and save-path management
 * @note Sprint 6 - version 1 save system foundation
 */

#include "core/SaveManager.h"
#include "core/LevelCatalog.h"

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

bool parseSaveData(std::istream& input, SaveData& data) {
    std::string key;
    while (input >> key) {
        if (key == "version") {
            if (!(input >> data.version)) return false;
        } else if (key == "highScore") {
            if (!(input >> data.highScore)) return false;
        } else if (key == "levelHighScore1") {
            if (!(input >> data.levelHighScores[0])) return false;
        } else if (key == "levelHighScore2") {
            if (!(input >> data.levelHighScores[1])) return false;
        } else if (key == "levelHighScore3") {
            if (!(input >> data.levelHighScores[2])) return false;
        } else if (key == "levelHighScore4") {
            if (!(input >> data.levelHighScores[3])) return false;
        } else if (key == "highestUnlockedLevel") {
            if (!(input >> data.highestUnlockedLevel)) return false;
        } else if (key == "soundVolume") {
            if (!(input >> data.soundVolume)) return false;
        } else if (key == "musicVolume") {
            if (!(input >> data.musicVolume)) return false;
        } else if (key == "matchCount") {
            int count = 0;
            input >> count;
        } else if (key == "match") {
            std::string line;
            if (std::getline(input, line)) {
                std::istringstream iss(line);
                std::vector<std::string> tokens;
                std::string tok;
                while (iss >> tok) {
                    tokens.push_back(tok);
                }
                try {
                    if (tokens.size() >= 7) {
                        GameRecord rec;
                        std::string safeDate = tokens[0];
                        std::replace(safeDate.begin(), safeDate.end(), '_', ' ');
                        rec.date = safeDate;
                        rec.level = std::stoi(tokens[1]);
                        std::string safeMode = tokens[2];
                        std::replace(safeMode.begin(), safeMode.end(), '_', ' ');
                        rec.mode = safeMode;
                        std::string safeHero = tokens[3];
                        std::replace(safeHero.begin(), safeHero.end(), '_', ' ');
                        rec.character = safeHero;
                        std::string safeRes = tokens[4];
                        std::replace(safeRes.begin(), safeRes.end(), '_', ' ');
                        rec.result = safeRes;
                        rec.score = std::stoi(tokens[5]);
                        rec.coins = std::stoi(tokens[6]);
                        data.matchHistory.push_back(rec);
                    } else if (tokens.size() >= 6) {
                        GameRecord rec;
                        std::string safeDate = tokens[0];
                        std::replace(safeDate.begin(), safeDate.end(), '_', ' ');
                        rec.date = safeDate;
                        rec.level = std::stoi(tokens[1]);
                        std::string safeHero = tokens[2];
                        std::replace(safeHero.begin(), safeHero.end(), '_', ' ');
                        rec.character = safeHero;
                        rec.mode = (rec.character == "CO-OP" || rec.character == "Co-op") ? "CO-OP" : "SOLO";
                        std::string safeRes = tokens[3];
                        std::replace(safeRes.begin(), safeRes.end(), '_', ' ');
                        rec.result = safeRes;
                        rec.score = std::stoi(tokens[4]);
                        rec.coins = std::stoi(tokens[5]);
                        data.matchHistory.push_back(rec);
                    }
                } catch (...) {
                    // Ignore corrupted record line to prevent load failures
                }
            }
        } else {
            std::string extra;
            input >> extra;
        }
    }
    return true;
}

bool hasValidSaveValues(const SaveData& data) {
    const bool soundVolumeValid = std::isfinite(data.soundVolume) &&
                                  data.soundVolume >= MIN_VOLUME &&
                                  data.soundVolume <= MAX_VOLUME;

    const bool musicVolumeValid = std::isfinite(data.musicVolume) &&
                                  data.musicVolume >= MIN_VOLUME &&
                                  data.musicVolume <= MAX_VOLUME;

    bool levelScoresValid = true;
    for (int s : data.levelHighScores) {
        if (s < 0) levelScoresValid = false;
    }

    return data.highScore >= 0 && levelScoresValid && data.highestUnlockedLevel >= 1 &&
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
    // Saves created against experimental catalogs must not unlock content
    // outside the locked Sprint 6 release graph.
    m_data.highestUnlockedLevel = std::min(
        m_data.highestUnlockedLevel, LevelCatalog::count() + 1);
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

bool SaveManager::updateHighScore(int globalScore, int stageScore, int level) {
    bool changed = false;

    if (globalScore > m_data.highScore) {
        m_data.highScore = globalScore;
        changed = true;
    }

    if (level >= 1 && level <= 4) {
        if (stageScore > m_data.levelHighScores[level - 1]) {
            m_data.levelHighScores[level - 1] = stageScore;
            changed = true;
        }
    }

    if (changed) {
        return save();
    }

    return false;
}

int SaveManager::getHighScore(int level) const {
    if (level >= 1 && level <= 4) {
        return m_data.levelHighScores[level - 1];
    }
    return m_data.highScore;
}

int SaveManager::getLevelHighScore(int level) const {
    if (level >= 1 && level <= 4) {
        return m_data.levelHighScores[level - 1];
    }
    return 0;
}

bool SaveManager::addGameRecord(const GameRecord& record) {
    m_data.matchHistory.insert(m_data.matchHistory.begin(), record);
    // Keep max 20 recent records
    if (m_data.matchHistory.size() > 20) {
        m_data.matchHistory.resize(20);
    }
    return save();
}

bool SaveManager::clearMatchHistory() {
    if (m_data.matchHistory.empty()) {
        return false;
    }
    m_data.matchHistory.clear();
    return save();
}

bool SaveManager::resetAllData() {
    const float prevSound = m_data.soundVolume;
    const float prevMusic = m_data.musicVolume;
    m_data = SaveData{};
    m_data.soundVolume = prevSound;
    m_data.musicVolume = prevMusic;
    return save();
}

const std::vector<GameRecord>& SaveManager::getMatchHistory() const {
    return m_data.matchHistory;
}

bool SaveManager::updateHighestUnlockedLevel(int level) {
    const int releaseLevel = std::clamp(level, 1, LevelCatalog::count() + 1);
    if (releaseLevel <= m_data.highestUnlockedLevel) {
        return false;
    }

    const int previousHighestUnlockedLevel = m_data.highestUnlockedLevel;

    m_data.highestUnlockedLevel = releaseLevel;

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

    if (std::filesystem::exists(m_savePath)) {
        std::filesystem::remove(m_savePath, errorCode);
        errorCode.clear();
    }

    std::filesystem::rename(temporaryPath, m_savePath, errorCode);

    if (errorCode) {
        errorCode.clear();
        std::filesystem::copy_file(temporaryPath, m_savePath,
                                   std::filesystem::copy_options::overwrite_existing, errorCode);
        std::error_code removeErr;
        std::filesystem::remove(temporaryPath, removeErr);
    }

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
    output << "levelHighScore1 " << m_data.levelHighScores[0] << std::endl;
    output << "levelHighScore2 " << m_data.levelHighScores[1] << std::endl;
    output << "levelHighScore3 " << m_data.levelHighScores[2] << std::endl;
    output << "levelHighScore4 " << m_data.levelHighScores[3] << std::endl;
    output << "highestUnlockedLevel " << m_data.highestUnlockedLevel << std::endl;
    output << "soundVolume " << m_data.soundVolume << std::endl;
    output << "musicVolume " << m_data.musicVolume << std::endl;

    output << "matchCount " << m_data.matchHistory.size() << std::endl;
    for (const auto& rec : m_data.matchHistory) {
        std::string safeDate = rec.date;
        std::replace(safeDate.begin(), safeDate.end(), ' ', '_');
        std::string safeMode = rec.mode;
        std::replace(safeMode.begin(), safeMode.end(), ' ', '_');
        std::string safeHero = rec.character;
        std::replace(safeHero.begin(), safeHero.end(), ' ', '_');
        std::string safeResult = rec.result;
        std::replace(safeResult.begin(), safeResult.end(), ' ', '_');
        output << "match " << safeDate << " " << rec.level << " " << safeMode
               << " " << safeHero << " " << safeResult << " " << rec.score
               << " " << rec.coins << std::endl;
    }

    output.flush();

    const bool writeSucceeded = output.good();
    output.close();

    return writeSucceeded;
}
