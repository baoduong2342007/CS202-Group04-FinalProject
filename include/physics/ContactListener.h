/**
 * @file ContactListener.h
 * @author TV3 (Bảo)
 * @brief ContactListener to resolve Box2D collisions and triggers
 * @note Week 2 Box2D integration
 */

#pragma once

#include <box2d/box2d.h>

class TileMap;

class ContactListener : public b2ContactListener {
public:
    explicit ContactListener(TileMap& tileMap);

    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;

    void PreSolve(
        b2Contact* contact,
        const b2Manifold* oldManifold
    ) override;

private:
    TileMap& m_tileMap;
};
