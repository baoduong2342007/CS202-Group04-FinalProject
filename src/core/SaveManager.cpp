/**
 * @file SaveManager.cpp
 * @author TV4
 * @brief Implementation of persistent game data and save-path management
 * @note Sprint 6 - version 1 save system foundation
 */

#include "core/SaveManager.h"

SaveManager::SaveManager(const std::string& savePath)
    : m_savePath(savePath),
      m_data{} {
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
