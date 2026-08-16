/**
 * @file BulletBillLauncher.cpp
 * @brief Bullet Bill launcher - proximity-triggered cannon spawner
 */

#include "entities/BulletBillLauncher.h"
#include "entities/BulletBill.h"

#include <cmath>

#include "core/TextureManager.h"

namespace {

// Turtle Cannon cells from the tileset atlas (docs/tileset_coordinate.md
// #74-76): top, body, and base, stacked bottom-up at the map cell.
const sf::IntRect CANNON_PARTS[] = {
    sf::IntRect({17, 50}, {16, 16}), // top
    sf::IntRect({34, 50}, {16, 16}), // body
    sf::IntRect({51, 50}, {16, 16})  // base
};
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
    if (!m_marioKnown || !m_world) {
        return;
    }

    const float dx = m_marioPosition.x - m_position.x;
    if (std::abs(dx) > RANGE) {
        return;
    }

    m_fireTimer += dt;
    if (m_fireTimer < FIRE_INTERVAL) {
        return;
    }
    m_fireTimer = 0.f;

    // Fire toward whichever side the player is standing on. The 3 s
    // interval stands in for the NES on-screen-bullet cap.
    const Direction direction = dx < 0.f ? Direction::LEFT : Direction::RIGHT;
    const float spawnX = direction == Direction::LEFT
                             ? m_position.x - 32.f
                             : m_position.x + m_size.x;
    m_pending.push_back(std::make_unique<BulletBill>(
        sf::Vector2f{spawnX, m_position.y + m_size.y - 32.f},
        m_world, m_theme, direction));
}

void BulletBillLauncher::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (!m_parts[0].has_value()) {
        // Same load-through-the-cache pattern Entity::setSprite uses.
        TextureManager& textures = TextureManager::getInstance();
        textures.loadTexture(TILESET_TEXTURE_PATH, TILESET_TEXTURE_PATH);
        const sf::Texture& texture = textures.getTexture(TILESET_TEXTURE_PATH);
        for (int i = 0; i < 3; ++i) {
            m_parts[i].emplace(texture);
            m_parts[i]->setTextureRect(CANNON_PARTS[i]);
            m_parts[i]->setScale({SPRITE_SCALE, SPRITE_SCALE});
        }
    }

    // Base sits on the map cell, body and top stack upward.
    const float tile = 16.f * SPRITE_SCALE;
    m_parts[2]->setPosition({m_position.x, m_position.y + 2 * tile}); // base
    m_parts[1]->setPosition({m_position.x, m_position.y + tile});     // body
    m_parts[0]->setPosition({m_position.x, m_position.y});            // top
    for (const auto& part : m_parts) {
        target.draw(*part, states);
    }
}

std::vector<std::unique_ptr<Entity>> BulletBillLauncher::takePendingSpawns() {
    std::vector<std::unique_ptr<Entity>> drained;
    drained.swap(m_pending);
    return drained;
}
