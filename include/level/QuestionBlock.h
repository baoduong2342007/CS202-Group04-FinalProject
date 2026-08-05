/**
 * @file QuestionBlock.h
 * @author TV5 (Truyền)
 * @brief Animated Question block entity — rendered from items_blocks.png spritesheet
 * @note Sprint 4 — spawned by EntityFactory from '?' tile code
 */

#pragma once

#include "entities/Entity.h"

class QuestionBlock : public Entity {
public:
    // 1. Constructor / Destructor
    QuestionBlock(const sf::Vector2f& position, b2World* world);
    ~QuestionBlock() override = default;

    // 2. Override methods
    void update(float dt) override;
};
