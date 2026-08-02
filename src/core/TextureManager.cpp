/**
 * @file TextureManager.cpp
 * @author TV2 (Nhật)
 * @brief Implementation of TextureManager.
 */

#include "core/TextureManager.h"
#include <iostream>

TextureManager::TextureManager() {
    constexpr unsigned int FALLBACK_TEXTURE_SIZE = 16;

    // Graceful Invariant Handling: Create a default fallback texture
    // If a requested texture is missing, we return a 16x16 solid magenta block
    // This prevents a crash and makes missing textures instantly visible on screen.
    auto fallback = std::make_unique<sf::Texture>();
    
    // SFML 3.0.0 might require specific image creation, but generally:
    const sf::Image fallbackImage(
        {FALLBACK_TEXTURE_SIZE, FALLBACK_TEXTURE_SIZE}, 
        sf::Color::Magenta
    );
    (void)fallback->loadFromImage(fallbackImage);
    
    m_textures["_fallback"] = std::move(fallback);
}

TextureManager::~TextureManager() {
    // std::unique_ptr automatically cleans up memory, no manual deletion needed here
}

bool TextureManager::loadTexture(const std::string& id, const std::string& filename) {
    // If it already exists, don't load it again
    if (m_textures.find(id) != m_textures.end()) {
        std::cerr << "[TextureManager] WARNING: Texture ID '" << id << "' already exists.\n";
        return true; 
    }

    auto texture = std::make_unique<sf::Texture>();
    
    if (!texture->loadFromFile(filename)) {
        std::cerr << "[TextureManager] ERROR: Failed to load file: " << filename << "\n";
        return false;
    }

    m_textures[id] = std::move(texture);
    return true;
}

const sf::Texture& TextureManager::getTexture(const std::string& id) const {
    auto it = m_textures.find(id);
    
    // Invariant Check: Does the ID actually exist?
    if (it == m_textures.end()) {
        std::cerr << "[TextureManager] ERROR: Texture ID '" << id << "' not found! Returning fallback.\n";
        return *m_textures.at("_fallback");
    }

    return *(it->second);
}

void TextureManager::deleteTexture(const std::string& id) {
    // Prevent deleting our safety fallback
    if (id == "_fallback") return;
    
    // .erase() safely removes the unique_ptr, which automatically deletes the sf::Texture
    m_textures.erase(id);
}