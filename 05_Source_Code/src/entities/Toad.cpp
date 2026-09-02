#include "entities/Toad.h"

#include "core/SpriteFrames_shared.h"

namespace {

constexpr const char* TOAD_TEXTURE = "assets/textures/items/items_objects.png";

const sf::Vector2f TOAD_SIZE{32.f, 48.f};

} // namespace

Toad::Toad(const sf::Vector2f& position)
: Entity({position.x, position.y - 16.f}, TOAD_SIZE) {
    // Source sprite is 16x24 and is rendered at 2x scale.
    // Shift upward by 16 px so Toad stands on the marker tile.
    setSprite(TOAD_TEXTURE);
}

void Toad::update(float dt) {
    (void)dt;

    if (!m_sprite) {
        return;
    }

    m_sprite->setTextureRect(SpriteFrames::legacy::Items::TOAD);

    m_sprite->setPosition(m_position);
    m_sprite->setScale({2.f, 2.f});
}
