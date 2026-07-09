/**
 * @file Entity.h
 * @author TV3 (Bảo)
 * @brief Base Entity class
 * @note Week 1 skeleton setup
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Entity : public sf::Drawable {
public:
    // 1. Constructor / Destructor
    Entity();
    Entity(const sf::Vector2f& position, const sf::Vector2f& size);
    ~Entity() override = default;

    // 2. Override methods
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    // 3. Public methods
    virtual void update(float dt) = 0;
    
    // TV2 (Engine & Render) will implement these in Entity.cpp
    void playAnimation(const std::string& clipName);
    void setSprite(const std::string& texturePath);

    // 4. Getters / Setters
    sf::FloatRect getBoundingBox() const;
    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const;

    void setPosition(const sf::Vector2f& position);
    void setVelocity(const sf::Vector2f& velocity);

protected:
    // 5. Protected methods
    virtual void updateBoundingBox();

    // 6. Protected members
    sf::FloatRect m_boundingBox;
    sf::Vector2f m_position;
    sf::Sprite m_sprite;
    sf::Vector2f m_velocity;
};
