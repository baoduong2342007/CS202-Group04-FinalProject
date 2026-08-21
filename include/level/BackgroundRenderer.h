/**
 * @file BackgroundRenderer.h
 * @author TV4 / TV1
 * @brief Themed background renderer with parallax scrolling and seamless wrapping
 * @note Designed for SFML 3 and 640x360 logical rendering resolution.
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <string_view>
#include "core/LevelCatalog.h"
#include "core/TextureManager.h"
#include "level/Camera.h"

struct AmbientParticle {
    sf::Vector2f position{0.f, 0.f};
    sf::Vector2f velocity{0.f, 0.f};
    float radius = 2.f;
    float lifetime = 0.f;
    float maxLifetime = 4.f;
    sf::Color baseColor{255, 255, 255, 255};
};

class BackgroundRenderer {
public:
    // 1. Constructor / Destructor
    BackgroundRenderer();
    ~BackgroundRenderer() = default;

    // Prevent copying
    BackgroundRenderer(const BackgroundRenderer&) = delete;
    BackgroundRenderer& operator=(const BackgroundRenderer&) = delete;

    // 2. Override methods
    // (None)

    // 3. Public methods (alphabetical)
    /**
     * @brief Initializes the renderer, preloading themed textures.
     * @param textureManager Reference to the centralized texture manager.
     * @param levelWidth Width of the level in pixels.
     * @param levelHeight Height of the level in pixels.
     */
    void init(TextureManager& textureManager, float levelWidth, float levelHeight);

    /**
     * @brief Renders the parallax background and ambient particle layers.
     * @param target Render target to draw onto.
     * @param camera Active camera view.
     */
    void render(sf::RenderTarget& target, const Camera& camera);

    /**
     * @brief Sets the active theme for background textures and ambient particles.
     * @param theme The new LevelTheme (OVERWORLD, UNDERGROUND, UNDERWATER, CASTLE).
     */
    void setTheme(LevelTheme theme);

    /**
     * @brief Updates ambient animation time and particles.
     * @param dt Delta time in seconds.
     */
    void update(float dt);

    // 4. Getters / Setters
    LevelTheme getTheme() const;
    float getScrollFactorX() const;
    void setScrollFactorX(float factor);

private:
    // 5. Private methods
    void initParticles();
    void updateParticles(float dt);
    void renderParticles(sf::RenderTarget& target, const Camera& camera);
    std::string getTexturePathForTheme(LevelTheme theme) const;

    // 6. Private members
    LevelTheme m_theme = LevelTheme::OVERWORLD;
    TextureManager* m_textureManager = nullptr;
    float m_levelWidth = 640.f;
    float m_levelHeight = 360.f;
    float m_animTime = 0.f;
    float m_scrollFactorX = 0.35f;

    std::vector<AmbientParticle> m_particles;
};
