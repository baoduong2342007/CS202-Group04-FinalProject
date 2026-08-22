/**
 * @file TextureManager.h
 * @author TV2 (Nhat)
 * @brief Singleton class for managing textures in the engine.
 * 
 * @details Implements a centralized resource management strategy using the 
 * Singleton design pattern. Ensures that textures are loaded only once and 
 * provides global access to entities. Adheres to strict memory ownership via 
 * smart pointers.
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>

/**
 * @class TextureManager
 * @brief Manages the loading, storage, and retrieval of sf::Texture resources.
 * 
 * @note Design Pattern: Singleton (Meyer's implementation).
 */
class TextureManager {
public:
    /**
     * @brief Gets the global Singleton instance of the TextureManager.
     * @return TextureManager& Reference to the static instance.
     */
    static TextureManager& getInstance();

    // Delete copy constructor and assignment operator to enforce Singleton property
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    /**
     * @brief Loads a texture from a file and stores it with the given ID.
     * 
     * @param id The unique string identifier for the texture.
     * @param filename The file path to the texture image.
     * @return true If the texture was successfully loaded or already exists.
     * @return false If the texture failed to load (e.g., file not found).
     */
    bool loadTexture(const std::string& id, const std::string& filename);

    /**
     * @brief Retrieves a loaded texture by its ID.
     * 
     * @param id The unique string identifier of the requested texture.
     * @return const sf::Texture& Reference to the requested texture, or a fallback if missing.
     */
    const sf::Texture& getTexture(const std::string& id) const;

    /**
     * @brief Checks if a texture with the given ID is currently loaded.
     * 
     * @param id The unique string identifier to check.
     * @return true If the texture exists in the manager.
     * @return false If the texture does not exist.
     */
    bool hasTexture(const std::string& id) const;

    /**
     * @brief Releases all managed GPU textures before process-wide
     * static destruction begins.
     */
    void shutdown();

private:
    /**
     * @brief Private constructor to enforce Singleton pattern.
     * Initializes the fallback texture to prevent crashes on missing resources.
     */
    TextureManager(); 
    
    /**
     * @brief Default private destructor.
     */
    ~TextureManager() = default;

    /** @brief Hash map storing unique pointers to textures, keyed by string IDs. */
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_textures;
};
