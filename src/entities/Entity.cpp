/**
 * @file Entity.cpp
 * @author TV3
 * @brief Base Entity class implementation with Box2D physics hooks
 */

#include "entities/Entity.h"
#include "physics/PhysicsEngine.h"
#include <iostream>

Entity::Entity()
    : m_boundingBox({0.f, 0.f}, {0.f, 0.f}),
      m_position(0.f, 0.f),
      m_size(0.f, 0.f),
      m_velocity(0.f, 0.f),
      m_body(nullptr) {}

Entity::Entity(const sf::Vector2f& position, const sf::Vector2f& size)
    : m_boundingBox(position, size),
      m_position(position),
      m_size(size),
      m_velocity(0.f, 0.f),
      m_body(nullptr) {}

Entity::~Entity() {
    if (m_body) {
        b2World* world = m_body->GetWorld();
        if (world) {
            world->DestroyBody(m_body);
        }
        m_body = nullptr;
    }
}

void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (m_sprite) {
        target.draw(*m_sprite, states);
    }
}

void Entity::initPhysics(b2BodyType type, const sf::Vector2f& size, bool isSensor) {
    b2World* world = PhysicsEngine::getInstance().getWorld();
    if (!world) {
        std::cerr << "Error: PhysicsEngine world not initialized!" << std::endl;
        return;
    }

    m_size = size;

    // 1. Define the body
    b2BodyDef bodyDef;
    bodyDef.type = type;
    // Set position to the center of the entity in Box2D meters
    bodyDef.position = PhysicsEngine::pixelsToMeters(m_position + size / 2.0f);
    bodyDef.fixedRotation = true; // Mario and enemies don't rotate
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this); // Store entity pointer in user data

    // 2. Create the body
    m_body = world->CreateBody(&bodyDef);

    // 3. Define the shape
    b2PolygonShape boxShape;
    float halfWidth = PhysicsEngine::pixelsToMeters(size.x / 2.0f);
    float halfHeight = PhysicsEngine::pixelsToMeters(size.y / 2.0f);
    boxShape.SetAsBox(halfWidth, halfHeight);

    // 4. Define the fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = (type == b2_dynamicBody) ? 1.0f : 0.0f;
    fixtureDef.friction = 0.2f;
    fixtureDef.isSensor = isSensor;

    // 5. Create the fixture
    m_body->CreateFixture(&fixtureDef);
}

void Entity::syncPhysics() {
    if (!m_body) return;

    b2Vec2 pos = m_body->GetPosition();
    b2Vec2 vel = m_body->GetLinearVelocity();

    // Box2D position is center of body. Convert back to top-left for SFML
    m_position = PhysicsEngine::metersToPixels(pos) - m_size / 2.0f;
    m_velocity = PhysicsEngine::metersToPixels(vel);

    if (m_sprite) {
        m_sprite->setPosition(m_position);
    }
    updateBoundingBox();
}

sf::FloatRect Entity::getBoundingBox() const {
    return sf::FloatRect(m_position, m_size);
}

sf::Vector2f Entity::getPosition() const {
    return m_position;
}

sf::Vector2f Entity::getVelocity() const {
    return m_velocity;
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

void Entity::updateBoundingBox() {
    m_boundingBox = sf::FloatRect(m_position, m_size);
}

// TV2 placeholders
void Entity::playAnimation(const std::string& clipName) {
    (void)clipName;
}

void Entity::setSprite(const std::string& texturePath) {
    (void)texturePath;
}
