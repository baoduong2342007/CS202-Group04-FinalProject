/**
 * @file BulletBillLauncher.cpp
 * @brief Bullet Bill launcher - proximity-triggered cannon spawner
 */

#include "entities/BulletBillLauncher.h"
#include "entities/BulletBill.h"
#include "entities/FireballExplosion.h"

#include <algorithm>
#include <cmath>

#include "core/TextureManager.h"

namespace {

struct CannonParts {
    sf::IntRect top;
    sf::IntRect body;
    sf::IntRect base;
};

CannonParts getCannonParts(LevelTheme theme) {
    switch (theme) {
        case LevelTheme::CASTLE:
            return {
                sf::IntRect({119, 102}, {16, 16}),
                sf::IntRect({119, 119}, {16, 16}),
                sf::IntRect({119, 136}, {16, 16})
            };
        case LevelTheme::UNDERGROUND:
            return {
                sf::IntRect({266, 17}, {16, 16}),
                sf::IntRect({266, 34}, {16, 16}),
                sf::IntRect({266, 51}, {16, 16})
            };
        case LevelTheme::UNDERWATER:
            return {
                sf::IntRect({266, 102}, {16, 16}),
                sf::IntRect({266, 119}, {16, 16}),
                sf::IntRect({266, 136}, {16, 16})
            };
        case LevelTheme::OVERWORLD:
        default:
            return {
                sf::IntRect({119, 17}, {16, 16}),
                sf::IntRect({119, 34}, {16, 16}),
                sf::IntRect({119, 51}, {16, 16})
            };
    }
}

constexpr const char* TILESET_TEXTURE_PATH = "assets/textures/tiles/tileset.png";
constexpr float SPRITE_SCALE = 2.f;

} // namespace

BulletBillLauncher::BulletBillLauncher(const sf::Vector2f& position,
                                       b2World* world,
                                       LevelTheme theme)
    : Entity(position, {32.f, 96.f}),
      m_world(world),
      m_theme(theme) {
    // Deliberately no physics body: the cannon is scenery, its bullets do
    // all the colliding.
}

void BulletBillLauncher::updateMarioPosition(const sf::Vector2f& marioPos) {
    m_marioPosition = marioPos;
    m_marioKnown = true;
}

void BulletBillLauncher::update(float dt) {
    if (m_ceaseFire || !m_marioKnown || !m_world) {
        return;
    }

    const sf::Vector2f topCenter{m_position.x + 16.f, m_position.y + 16.f};
    const float dx = m_marioPosition.x - topCenter.x;
    const float dy = (m_marioPosition.y + 16.f) - topCenter.y;

    if (std::abs(dx) > RANGE) {
        return;
    }

    // Dynamic AI tracking angle toward Mario (clamped to +/-20 degrees for steady long-range turret elevation)
    const float rawPitch = std::atan2(dy, std::abs(dx)) * 180.f / 3.14159265358979323846f;
    const float clampedPitch = std::clamp(rawPitch, -20.f, 20.f);

    const Direction direction = dx < 0.f ? Direction::LEFT : Direction::RIGHT;
    if (direction == Direction::LEFT) {
        m_currentAimAngle = 180.f - clampedPitch;
    } else {
        m_currentAimAngle = clampedPitch;
    }

    m_fireTimer += dt;
    if (m_fireTimer < FIRE_INTERVAL) {
        return;
    }
    m_fireTimer = 0.f;

    const float rad = m_currentAimAngle * 3.14159265358979323846f / 180.f;
    const float spawnX = topCenter.x + 20.f * std::cos(rad) - 16.f;
    const float spawnY = topCenter.y + 20.f * std::sin(rad) - 16.f;

    m_pending.push_back(std::make_unique<BulletBill>(
        sf::Vector2f{spawnX, spawnY},
        m_world, m_theme, direction, m_currentAimAngle));
}

void BulletBillLauncher::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    const CannonParts parts = getCannonParts(m_theme);
    if (!m_parts[0].has_value()) {
        // Same load-through-the-cache pattern Entity::setSprite uses.
        TextureManager& textures = TextureManager::getInstance();
        textures.loadTexture(TILESET_TEXTURE_PATH, TILESET_TEXTURE_PATH);
        const sf::Texture& texture = textures.getTexture(TILESET_TEXTURE_PATH);
        
        m_parts[0].emplace(texture);
        m_parts[0]->setTextureRect(parts.top);
        m_parts[0]->setOrigin({8.f, 8.f});
        m_parts[0]->setScale({SPRITE_SCALE, SPRITE_SCALE});

        m_parts[1].emplace(texture);
        m_parts[1]->setTextureRect(parts.body);
        m_parts[1]->setScale({SPRITE_SCALE, SPRITE_SCALE});

        m_parts[2].emplace(texture);
        m_parts[2]->setTextureRect(parts.base);
        m_parts[2]->setScale({SPRITE_SCALE, SPRITE_SCALE});
    }

    const float tile = 16.f * SPRITE_SCALE;
    const sf::Vector2f hingePos{m_position.x + 16.f, m_position.y + 24.f};

    // Base (part 2) and Body (part 1) remain steady and upright
    m_parts[2]->setPosition({m_position.x, m_position.y + 2 * tile});
    m_parts[1]->setPosition({m_position.x, m_position.y + tile});
    target.draw(*m_parts[2], states);
    target.draw(*m_parts[1], states);

    // Mechanical Swivel Collar (cushion joint pad connecting body to swivel turret)
    sf::RectangleShape darkPlate({24.f, 6.f});
    darkPlate.setOrigin({12.f, 3.f});
    darkPlate.setPosition(hingePos);
    darkPlate.setFillColor(sf::Color(35, 38, 45));
    target.draw(darkPlate, states);

    sf::RectangleShape metalRing({18.f, 3.f});
    metalRing.setOrigin({9.f, 1.5f});
    metalRing.setPosition(hingePos);
    if (m_theme == LevelTheme::CASTLE || m_theme == LevelTheme::UNDERGROUND) {
        metalRing.setFillColor(sf::Color(170, 170, 180));
    } else {
        metalRing.setFillColor(sf::Color(180, 140, 90));
    }
    target.draw(metalRing, states);

    // Top Swivel Turret (part 0) rotates around the hinge joint eyelet {8.f, 12.f}
    const float rad = m_currentAimAngle * 3.14159265358979323846f / 180.f;
    const bool aimsLeft = std::cos(rad) <= 0.f;
    
    m_parts[0]->setOrigin({8.f, 12.f});
    m_parts[0]->setPosition(hingePos);
    if (aimsLeft) {
        m_parts[0]->setScale({SPRITE_SCALE, SPRITE_SCALE});
        m_parts[0]->setRotation(sf::degrees(m_currentAimAngle - 180.f));
    } else {
        m_parts[0]->setScale({-SPRITE_SCALE, SPRITE_SCALE});
        m_parts[0]->setRotation(sf::degrees(m_currentAimAngle));
    }
    target.draw(*m_parts[0], states);

    // When just fired (first 0.2s), draw a muzzle flash smoke puff at the rotated cannon opening
    if (m_fireTimer < 0.2f && m_parts[0].has_value()) {
        const sf::Vector2f muzzlePos = hingePos + sf::Vector2f{28.f * std::cos(rad), 28.f * std::sin(rad) - 6.f};
        sf::Sprite puff = *m_parts[0];
        puff.setTextureRect(parts.top);
        puff.setColor(sf::Color(255, 255, 255, static_cast<uint8_t>(255 * (1.f - m_fireTimer / 0.2f))));
        puff.setPosition(muzzlePos);
        puff.setRotation(m_parts[0]->getRotation());
        target.draw(puff, states);
    }
}

std::vector<std::unique_ptr<Entity>> BulletBillLauncher::takePendingSpawns() {
    std::vector<std::unique_ptr<Entity>> drained;
    drained.swap(m_pending);
    return drained;
}
