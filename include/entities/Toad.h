#pragma once

#include "entities/Entity.h"

class Toad : public Entity {
public:
    explicit Toad(const sf::Vector2f& position);
    ~Toad() override = default;

    void update(float dt) override;

    EntityType getType() const override {
        return EntityType::TERRAIN;
    }

    bool isToad() const override {
        return true;
    }
};
