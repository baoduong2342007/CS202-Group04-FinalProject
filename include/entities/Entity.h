/**
 * @file Entity.h
 * @author TV3 (Bảo)
 * @brief Base Entity class with Box2D physics memory management
 * @note Week 7 update
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <string>
#include <optional>

class Entity : public sf::Drawable {
public:
    // 1. Constructor / Destructor
    Entity();
    Entity(const sf::Vector2f& position, const sf::Vector2f& size);
    ~Entity() override; // Safely destroys Box2D body

    // 2. Override methods
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    // 3. Public methods
    virtual void update(float dt) = 0;

    void playAnimation(const std::string& clipName);
    void setSprite(const std::string& texturePath);

    // Box2D Physics Methods
    void initPhysics(b2BodyType type, const sf::Vector2f& size, bool isSensor = false);
    virtual void syncPhysics();
    void destroyPhysicsBody();

    // 4. Getters / Setters
    sf::FloatRect getBoundingBox() const;
    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const;

    void setPosition(const sf::Vector2f& position);
    void setVelocity(const sf::Vector2f& velocity);

    b2Body* getBody() const { return m_body; }

    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }

    bool isPendingDestroy() const { return m_pendingDestroy; }
    void markForDestroy() { m_pendingDestroy = true; m_active = false; }

protected:
    // 5. Protected methods
    virtual void updateBoundingBox();

    // 6. Protected members
    sf::FloatRect m_boundingBox;
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    std::optional<sf::Sprite> m_sprite;
    sf::Vector2f m_velocity;

    // Box2D Body
    b2Body* m_body = nullptr;
    bool m_active = true;
    bool m_pendingDestroy = false;
};
