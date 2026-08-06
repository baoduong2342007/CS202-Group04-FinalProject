/**
 * @file Entity.cpp
 * @author TV1 (Dương) & TV3 (Bảo)
 * @brief Base Entity class implementation with Box2D physics, rendering, and safe lifecycle management
 * @note Sprint 4: TextureManager + AnimationSystem functional; safe Box2D body destruction
 */

#include "entities/Entity.h"

#include <iostream>

#include "core/AnimationSystem.h"
#include "core/TextureManager.h"
#include "physics/PhysicsEngine.h"

Entity::Entity()
    : m_boundingBox({0.f, 0.f}, {0.f, 0.f}),
      m_position(0.f, 0.f),
      m_size(0.f, 0.f),
      m_velocity(0.f, 0.f),
      m_body(nullptr),
      m_markedForRemoval(false),
      m_active(true),
      m_pendingDestroy(false) {
    m_animationSystem = std::make_unique<AnimationSystem>();
}

Entity::Entity(const sf::Vector2f& position, const sf::Vector2f& size)
    : m_boundingBox(position, size),
      m_position(position),
      m_size(size),
      m_velocity(0.f, 0.f),
      m_body(nullptr),
      m_markedForRemoval(false),
      m_active(true),
      m_pendingDestroy(false) {
    m_animationSystem = std::make_unique<AnimationSystem>();
}

Entity::~Entity() {
    destroyPhysicsBody();
}

void Entity::destroyPhysicsBody() {
    if (m_body) {
        b2World* world = m_body->GetWorld();
        if (world) {
            // Nullify user data pointer before destroying to prevent dangling reference
            m_body->GetUserData().pointer = 0;
            world->DestroyBody(m_body);
        }
        m_body = nullptr;
    }
}

void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (m_active && m_sprite) {
        target.draw(*m_sprite, states);
    }
}

// ── TextureManager + Sprite Integration ────────────────────────

void Entity::setTextureManager(TextureManager& textureManager) {
    m_textureManager = &textureManager;
    if (!m_textureId.empty()) {
        setSprite(m_textureId);
    }
}

void Entity::setSprite(const std::string& textureId) {
    m_textureId = textureId;

    if (!m_textureManager) {
        // TextureManager not set yet — will be loaded when setTextureManager() is called
        return;
    }

    // Load texture into TextureManager if not already cached
    if (!m_textureManager->hasTexture(textureId)) {
        // Use the textureId as both the cache key and the file path
        if (!m_textureManager->loadTexture(textureId, textureId)) {
            std::cerr << "[Entity] ERROR: Failed to load texture '"
                      << textureId << "'. Using fallback.\n";
        }
    }

    // Create or update the sprite with the loaded texture
    const sf::Texture& texture = m_textureManager->getTexture(textureId);
    m_sprite.emplace(texture);
    m_sprite->setPosition(m_position);
}

// ── Animation Integration ──────────────────────────────────────

void Entity::playAnimation(const std::string& clipName) {
    if (!m_animationSystem) {
        // No animation system attached — silently ignore
        return;
    }
    m_animationSystem->play(clipName);
}

void Entity::updateAnimation(float dt) {
    if (!m_animationSystem || !m_sprite) {
        return;
    }
    m_animationSystem->update(dt, *m_sprite);
}

// ── Box2D Physics ──────────────────────────────────────────────

void Entity::initPhysics(b2World* world, b2BodyType type, const sf::Vector2f& size, bool isSensor) {
    if (!world) {
#ifdef DEBUG
        std::cerr << "[DEBUG][Entity] PhysicsEngine world not initialized!" << std::endl;
#endif
        return;
    }

    // Clear previous physics body if exists
    destroyPhysicsBody();

    m_size = size;

    // 1. Define the body
    b2BodyDef bodyDef;
    bodyDef.type = type;
    bodyDef.position = PhysicsEngine::pixelsToMeters(m_position + size / 2.0f);
    bodyDef.fixedRotation = true;
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);

    // 2. Create the body
    m_body = world->CreateBody(&bodyDef);

    // 3. Define the shape
    b2PolygonShape boxShape;
    float halfWidth = PhysicsEngine::pixelsToMeters(size.x / 2.0f);
    float halfHeight = PhysicsEngine::pixelsToMeters(size.y / 2.0f);
    boxShape.SetAsBox(halfWidth, halfHeight);

    // 4. Define the fixture (Zero friction for dynamic bodies to prevent wall-sticking)
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = (type == b2_dynamicBody) ? 1.0f : 0.0f;
    fixtureDef.friction = (type == b2_dynamicBody) ? 0.0f : 0.2f;
    fixtureDef.isSensor = isSensor;

    // 5. Create the fixture
    m_body->CreateFixture(&fixtureDef);
}

void Entity::syncPhysics() {
    if (!m_body) return;

    b2Vec2 pos = m_body->GetPosition();
    b2Vec2 vel = m_body->GetLinearVelocity();

    m_position = PhysicsEngine::metersToPixels(pos) - m_size / 2.0f;
    m_velocity = PhysicsEngine::metersToPixels(vel);

    if (m_sprite) {
        m_sprite->setPosition(m_position);

        // Auto-scale sprite to match entity size if textureRect is set
        sf::IntRect texRect = m_sprite->getTextureRect();
        if (texRect.size.x > 0 && texRect.size.y > 0) {
            m_sprite->setScale({m_size.x / static_cast<float>(texRect.size.x),
                                m_size.y / static_cast<float>(texRect.size.y)});
        }
    }
    updateBoundingBox();
}

// ── Getters / Setters ──────────────────────────────────────────

sf::FloatRect Entity::getBoundingBox() const {
    return sf::FloatRect(m_position, m_size);
}

sf::Vector2f Entity::getPosition() const {
    return m_position;
}

sf::Vector2f Entity::getVelocity() const {
    return m_velocity;
}

bool Entity::shouldRemove() const {
    return m_markedForRemoval;
}

void Entity::setPosition(const sf::Vector2f& position) {
    m_position = position;
    if (m_body) {
        m_body->SetTransform(PhysicsEngine::pixelsToMeters(position + m_size / 2.0f), m_body->GetAngle());
    }
    if (m_sprite) {
        m_sprite->setPosition(m_position);
    }
    updateBoundingBox();
}

void Entity::setVelocity(const sf::Vector2f& velocity) {
    m_velocity = velocity;
    if (m_body) {
        m_body->SetLinearVelocity(PhysicsEngine::pixelsToMeters(velocity));
    }
}

void Entity::markForRemoval() {
    m_markedForRemoval = true;
}

void Entity::updateBoundingBox() {
    m_boundingBox = sf::FloatRect(m_position, m_size);
}

// ── Polymorphic Collision Dispatch Defaults ─────────────────────

void Entity::onCollisionBegin(Entity* other, b2Contact* contact, const b2Vec2& normal) {
    (void)other;
    (void)contact;
    (void)normal;
}

void Entity::onCollisionEnd(Entity* other, b2Contact* contact) {
    (void)other;
    (void)contact;
}


