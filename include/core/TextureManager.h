
#pragma once

/**
 * @file TextureManager.h
 * @author TV2 (Nhật)
 * @brief Manages loading, caching, and retrieving sf::Texture objects to prevent redundant disk reads.
 * @note DANGER: Before calling deleteTexture(), you MUST ensure that no active sf::Sprite is holding a reference to the texture being deleted. Deleting a texture while a sprite still points to it will cause a segmentation fault!
 */

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>

class TextureManager {
public:
    // 1. Constructor / Destructor
    TextureManager();
    ~TextureManager();

    // Prevent copying to ensure only one manager instance handles resources
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    // 2. Overrides
    // (None currently required for this class)

    // 3. Public methods
    /**
     * @brief Loads a texture from file and caches it with a string ID.
     * @param id The string ID to associate with the texture.
     * @param filename The file path to the texture.
     * @return true if loading succeeds, false otherwise.
     */
    bool loadTexture(const std::string& id, const std::string& filename);

    /**
     * @brief Deletes a texture from the manager to free up memory.
     * 
     * =====================================================================
     * CRITICAL WARNING: sf::Sprite does NOT own its texture! It only holds 
     * a raw pointer to it. You MUST destroy or reassign any sf::Sprite using 
     * this texture BEFORE calling this function. Otherwise, the game will 
     * crash instantly with a dangling pointer error (segmentation fault).
     * =====================================================================
     * 
     * @param id The string ID of the texture to delete.
     */
    void deleteTexture(const std::string& id);

    // 4. Getters / Setters
    /**
     * @brief Retrieves a reference to a cached texture by its ID.
     * @param id The string ID of the texture.
     * @return const sf::Texture& reference to the texture (or fallback texture).
     */
    const sf::Texture& getTexture(const std::string& id) const;

private:
    // 5. Private methods
    // (None currently required for this class)

    // 6. Private members
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_textures;
};