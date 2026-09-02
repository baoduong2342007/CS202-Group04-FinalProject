/**
 * @file BackgroundRenderer.cpp
 * @author TV4 / TV1
 * @brief Implementation of themed parallax background renderer with seamless wrapping.
 */

#include "level/BackgroundRenderer.h"
#include "core/DisplayConfig.h"
#include "core/SpriteFrames_shared.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

namespace {

constexpr float BASE_BG_WIDTH = 640.f;
constexpr float BASE_BG_HEIGHT = 360.f;
constexpr std::size_t MAX_PARTICLES = 25;

float randomFloat(float minVal, float maxVal) {
    float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    return minVal + r * (maxVal - minVal);
}

} // namespace

BackgroundRenderer::BackgroundRenderer() = default;

void BackgroundRenderer::init(TextureManager& textureManager, float levelWidth, float levelHeight) {
    m_textureManager = &textureManager;
    m_levelWidth = std::max(levelWidth, BASE_BG_WIDTH);
    m_levelHeight = std::max(levelHeight, BASE_BG_HEIGHT);

    // Preload all 4 background textures into the TextureManager
    m_textureManager->loadTexture(
        std::string(SpriteFrames::legacy::Backgrounds::OVERWORLD_PATH),
        std::string(SpriteFrames::legacy::Backgrounds::OVERWORLD_PATH));
    m_textureManager->loadTexture(
        std::string(SpriteFrames::legacy::Backgrounds::UNDERGROUND_PATH),
        std::string(SpriteFrames::legacy::Backgrounds::UNDERGROUND_PATH));
    m_textureManager->loadTexture(
        std::string(SpriteFrames::legacy::Backgrounds::UNDERWATER_PATH),
        std::string(SpriteFrames::legacy::Backgrounds::UNDERWATER_PATH));
    m_textureManager->loadTexture(
        std::string(SpriteFrames::legacy::Backgrounds::CASTLE_PATH),
        std::string(SpriteFrames::legacy::Backgrounds::CASTLE_PATH));

    initParticles();
}

void BackgroundRenderer::setTheme(LevelTheme theme) {
    if (m_theme != theme) {
        m_theme = theme;
        initParticles();
    }
}

void BackgroundRenderer::update(float dt) {
    m_animTime += dt;
    updateParticles(dt);
}

void BackgroundRenderer::render(sf::RenderTarget& target, const Camera& camera) {
    const sf::View& view = camera.getView();
    const sf::Vector2f center = view.getCenter();
    const sf::Vector2f viewSize = view.getSize();

    const float camLeft = center.x - viewSize.x * 0.5f;
    const float camTop = center.y - viewSize.y * 0.5f;

    std::string texPath = getTexturePathForTheme(m_theme);

    if (m_textureManager && m_textureManager->hasTexture(texPath)) {
        const sf::Texture& tex = m_textureManager->getTexture(texPath);
        sf::Sprite sprite(tex);
        sprite.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(BASE_BG_WIDTH), static_cast<int>(BASE_BG_HEIGHT)}));

        // Parallax horizontal wrapping calculation
        // Foreground shifts by camLeft; background shifts at rate m_scrollFactorX
        float rawShift = camLeft * m_scrollFactorX;
        float layerShiftX = std::fmod(rawShift, BASE_BG_WIDTH);
        if (layerShiftX < 0.f) {
            layerShiftX += BASE_BG_WIDTH;
        }

        // Anchor background vertically to camera viewport with subtle vertical offset
        float bgY = camTop;

        // Draw seamless strips from left to right covering the full visible view
        float startX = camLeft - layerShiftX - BASE_BG_WIDTH;
        float endX = camLeft + viewSize.x + BASE_BG_WIDTH;

        for (float x = startX; x <= endX; x += BASE_BG_WIDTH) {
            sprite.setPosition(sf::Vector2f(x, bgY));
            target.draw(sprite);
        }
    } else {
        // Safe fallback solid color if texture is missing
        sf::RectangleShape fallback(viewSize);
        fallback.setPosition(sf::Vector2f(camLeft, camTop));
        sf::Color fallbackCol;
        switch (m_theme) {
            case LevelTheme::UNDERGROUND:
                fallbackCol = sf::Color(16, 18, 34);
                break;
            case LevelTheme::UNDERWATER:
                fallbackCol = sf::Color(8, 25, 75);
                break;
            case LevelTheme::CASTLE:
                fallbackCol = sf::Color(18, 14, 18);
                break;
            case LevelTheme::OVERWORLD:
            default:
                fallbackCol = sf::Color(92, 148, 252);
                break;
        }
        fallback.setFillColor(fallbackCol);
        target.draw(fallback);
    }

    // Render ambient particle layer
    renderParticles(target, camera);
}

LevelTheme BackgroundRenderer::getTheme() const {
    return m_theme;
}

float BackgroundRenderer::getScrollFactorX() const {
    return m_scrollFactorX;
}

void BackgroundRenderer::setScrollFactorX(float factor) {
    m_scrollFactorX = factor;
}

void BackgroundRenderer::initParticles() {
    m_particles.clear();
    m_particles.resize(MAX_PARTICLES);

    for (std::size_t i = 0; i < m_particles.size(); ++i) {
        AmbientParticle& p = m_particles[i];
        p.position = sf::Vector2f(randomFloat(0.f, BASE_BG_WIDTH), randomFloat(0.f, BASE_BG_HEIGHT));
        p.lifetime = randomFloat(0.f, 4.f);
        p.maxLifetime = randomFloat(2.5f, 5.f);

        switch (m_theme) {
            case LevelTheme::UNDERWATER:
                // Rising ocean bubbles
                p.velocity = sf::Vector2f(randomFloat(-4.f, 4.f), randomFloat(-30.f, -55.f));
                p.radius = randomFloat(1.5f, 3.5f);
                p.baseColor = sf::Color(210, 245, 255, 170);
                break;

            case LevelTheme::CASTLE:
                // Rising fiery embers / sparks
                p.velocity = sf::Vector2f(randomFloat(-12.f, 12.f), randomFloat(-25.f, -60.f));
                p.radius = randomFloat(1.0f, 2.5f);
                p.baseColor = ((i % 2) == 0) ? sf::Color(255, 160, 30, 200)
                                             : sf::Color(255, 70, 15, 180);
                break;

            case LevelTheme::UNDERGROUND:
                // Shimmering crystal dust
                p.velocity = sf::Vector2f(randomFloat(-5.f, 5.f), randomFloat(-8.f, 8.f));
                p.radius = randomFloat(1.0f, 2.0f);
                p.baseColor = ((i % 2) == 0) ? sf::Color(0, 240, 255, 160)
                                             : sf::Color(220, 100, 255, 150);
                break;

            case LevelTheme::OVERWORLD:
            default:
                // Gentle floating pollen / breeze motes
                p.velocity = sf::Vector2f(randomFloat(15.f, 35.f), randomFloat(-5.f, 5.f));
                p.radius = randomFloat(1.0f, 2.0f);
                p.baseColor = sf::Color(255, 255, 255, 130);
                break;
        }
    }
}

void BackgroundRenderer::updateParticles(float dt) {
    for (std::size_t i = 0; i < m_particles.size(); ++i) {
        AmbientParticle& p = m_particles[i];
        p.lifetime += dt;

        // Wave motion
        float waveOffset = std::sin(m_animTime * 2.f + static_cast<float>(i)) * 6.f;
        p.position.x += (p.velocity.x + waveOffset) * dt;
        p.position.y += p.velocity.y * dt;

        // Wrap particles within [0, BASE_BG_WIDTH] and [0, BASE_BG_HEIGHT]
        if (p.position.x < 0.f) p.position.x += BASE_BG_WIDTH;
        if (p.position.x > BASE_BG_WIDTH) p.position.x -= BASE_BG_WIDTH;

        if (p.position.y < 0.f) {
            p.position.y += BASE_BG_HEIGHT;
            p.position.x = randomFloat(0.f, BASE_BG_WIDTH);
        } else if (p.position.y > BASE_BG_HEIGHT) {
            p.position.y -= BASE_BG_HEIGHT;
            p.position.x = randomFloat(0.f, BASE_BG_WIDTH);
        }

        if (p.lifetime >= p.maxLifetime) {
            p.lifetime = 0.f;
            p.position.y = (p.velocity.y < 0.f) ? BASE_BG_HEIGHT : 0.f;
            p.position.x = randomFloat(0.f, BASE_BG_WIDTH);
        }
    }
}

void BackgroundRenderer::renderParticles(sf::RenderTarget& target, const Camera& camera) {
    const sf::View& view = camera.getView();
    const sf::Vector2f center = view.getCenter();
    const sf::Vector2f viewSize = view.getSize();

    const float camLeft = center.x - viewSize.x * 0.5f;
    const float camTop = center.y - viewSize.y * 0.5f;

    sf::CircleShape shape;

    for (const auto& p : m_particles) {
        // Calculate particle alpha fade based on lifetime
        float progress = p.lifetime / p.maxLifetime;
        float alphaFactor = std::sin(progress * 3.14159265f); // Fade in then fade out
        if (alphaFactor < 0.f) alphaFactor = 0.f;

        sf::Color c = p.baseColor;
        c.a = static_cast<uint8_t>(static_cast<float>(p.baseColor.a) * alphaFactor);

        shape.setRadius(p.radius);
        shape.setOrigin(sf::Vector2f(p.radius, p.radius));
        shape.setFillColor(c);

        // Position relative to the viewport
        shape.setPosition(sf::Vector2f(camLeft + p.position.x, camTop + p.position.y));
        target.draw(shape);
    }
}

std::string BackgroundRenderer::getTexturePathForTheme(LevelTheme theme) const {
    switch (theme) {
        case LevelTheme::UNDERGROUND:
            return std::string(SpriteFrames::legacy::Backgrounds::UNDERGROUND_PATH);
        case LevelTheme::UNDERWATER:
            return std::string(SpriteFrames::legacy::Backgrounds::UNDERWATER_PATH);
        case LevelTheme::CASTLE:
            return std::string(SpriteFrames::legacy::Backgrounds::CASTLE_PATH);
        case LevelTheme::OVERWORLD:
        default:
            return std::string(SpriteFrames::legacy::Backgrounds::OVERWORLD_PATH);
    }
}
