/**
 * @file Entity.h
 * @author TV3 (Bảo)
 * @brief Base Entity class
 * @note Week 1 skeleton setup
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
    ~Entity() override; // Destroy Box2D body

    // 2. Override methods
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    // 3. Public methods
    virtual void update(float dt) = 0;
    
    // TV2 (Engine & Render) will implement these in Entity.cpp
    void playAnimation(const std::string& clipName);
    void setSprite(const std::string& texturePath);

    // Box2D Physics Methods
    void initPhysics(b2BodyType type, const sf::Vector2f& size, bool isSensor = false);
    virtual void syncPhysics();

    // 4. Getters / Setters
    sf::FloatRect getBoundingBox() const;
    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const;

    void setPosition(const sf::Vector2f& position);
    void setVelocity(const sf::Vector2f& velocity);
    
    b2Body* getBody() const { return m_body; }

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
};
