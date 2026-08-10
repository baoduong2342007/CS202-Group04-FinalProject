/**
 * @file TextureManager.cpp
 * @author TV2 (Nhật)
 * @brief Implementation of the TextureManager class.
 * 
 * @details Fixed for SFML 3.0.0 API. sf::Texture has no loadFromFile() in SFML 3, 
 * utilizing the constructor sf::Texture(filename) which throws on failure.
 * Refactored to implement the Singleton design pattern.
 */

#include "core/TextureManager.h"
#include <iostream>

TextureManager& TextureManager::getInstance() {
    static TextureManager instance;
    return instance;
}

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
        return true;
    }

    try {
        // Background textures (e.g. bg_mountains.png) should not mask out sky blue colors
        if (id.find("bg_") != std::string::npos || filename.find("bg_") != std::string::npos) {
            auto texture = std::make_unique<sf::Texture>(filename);
            m_textures[id] = std::move(texture);
            return true;
        }

        sf::Image img;
        if (img.loadFromFile(filename)) {
            sf::Color bg1(146, 144, 255);
            sf::Color bg2(0, 41, 140);
            sf::Color corner = img.getPixel({0, 0});
            
            // Create mask for background colors to make them transparent
            img.createMaskFromColor(bg1);
            img.createMaskFromColor(sf::Color(148, 148, 255));
            img.createMaskFromColor(bg2);
            if (corner.a == 255 && (corner == bg1 || corner == bg2 || (corner.r < 50 && corner.g < 50))) {
                img.createMaskFromColor(corner);
            }

            auto texture = std::make_unique<sf::Texture>(img);
            m_textures[id] = std::move(texture);
            return true;
        }

        // Fallback direct load
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

    // If texture not found in map, attempt to auto-load using id as file path
    if (it == m_textures.end()) {
        const_cast<TextureManager*>(this)->loadTexture(id, id);
        it = m_textures.find(id);
    }

    // Invariant Check: Does the ID actually exist?
    if (it == m_textures.end()) {
        std::cerr << "[TextureManager] ERROR: Texture ID '" << id
                  << "' not found! Returning fallback.\n";
        auto fallbackIt = m_textures.find("_fallback");
        if (fallbackIt != m_textures.end()) {
            return *(fallbackIt->second);
        }
        // CRITICAL: If even the fallback is missing, the TextureManager is in an
        // unrecoverable state. Throw an exception instead of using a static sf::Texture
        // to prevent silent OpenGL segmentation faults on exit.
        throw std::runtime_error("[TextureManager] FATAL: No fallback texture available!");
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
