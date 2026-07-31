/**
 * @file TextureManager.cpp
 * @author TV2 (Nhật)
 * @brief Implementation of TextureManager — fixed for SFML 3.0.0 API.
 * @note Sprint 4 fix: sf::Texture has NO loadFromFile() in SFML 3.
 *       Must use constructor sf::Texture(filename) which throws on failure.
 */

#include "core/TextureManager.h"

#include <iostream>

TextureManager::TextureManager() {
    constexpr unsigned int FALLBACK_TEXTURE_SIZE = 16;

    // Graceful Invariant Handling: Create a default fallback texture
    // If a requested texture is missing, we return a 16x16 solid magenta block
    // This prevents a crash and makes missing textures instantly visible on screen.
    try {
        const sf::Image fallbackImage(
            {FALLBACK_TEXTURE_SIZE, FALLBACK_TEXTURE_SIZE},
            sf::Color::Magenta
        );
        // SFML 3: sf::Texture(Image) constructor (throws on failure)
        auto fallback = std::make_unique<sf::Texture>(fallbackImage);
        m_textures["_fallback"] = std::move(fallback);
    } catch (const sf::Exception& e) {
        std::cerr << "[TextureManager] WARNING: Failed to create fallback texture: "
                  << e.what() << "\n";
    }
}



bool TextureManager::loadTexture(const std::string& id, const std::string& filename) {
    // If it already exists, don't load it again
    if (m_textures.find(id) != m_textures.end()) {
        std::cerr << "[TextureManager] WARNING: Texture ID '" << id << "' already exists.\n";
        return true;
    }

    try {
        // SFML 3: sf::Texture(filename) constructor (throws sf::Exception on failure)
        auto texture = std::make_unique<sf::Texture>(filename);
        m_textures[id] = std::move(texture);
        return true;
    } catch (const sf::Exception& e) {
        std::cerr << "[TextureManager] ERROR: Failed to load '" << filename
                  << "': " << e.what() << "\n";
        return false;
    }
}

const sf::Texture& TextureManager::getTexture(const std::string& id) const {
    auto it = m_textures.find(id);

    // Invariant Check: Does the ID actually exist?
    if (it == m_textures.end()) {
        std::cerr << "[TextureManager] ERROR: Texture ID '" << id
                  << "' not found! Returning fallback.\n";
        auto fallbackIt = m_textures.find("_fallback");
        if (fallbackIt != m_textures.end()) {
            return *(fallbackIt->second);
        }
        // CRITICAL: If even the fallback is missing, the TextureManager is in an
        // unrecoverable state. Create a last-resort Magenta texture so the game
        // does not crash, but makes the problem highly visible on screen.
        std::cerr << "[TextureManager] FATAL: No fallback texture! "
                  << "Creating emergency 16x16 Magenta texture.\n";
        constexpr unsigned int EMERGENCY_SIZE = 16;
        static const sf::Texture emergencyFallback(sf::Image(
            {EMERGENCY_SIZE, EMERGENCY_SIZE}, sf::Color::Magenta));
        return emergencyFallback;
    }

    return *(it->second);
}

bool TextureManager::hasTexture(const std::string& id) const {
    return m_textures.find(id) != m_textures.end();
}

void TextureManager::deleteTexture(const std::string& id) {
    // Prevent deleting our safety fallback
    if (id == "_fallback") return;

    // .erase() safely removes the unique_ptr, which automatically deletes the sf::Texture
    m_textures.erase(id);
}