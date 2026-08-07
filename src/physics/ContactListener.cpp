/**
 * @file ContactListener.cpp
 * @author TV3
 * @brief ContactListener implementation delegating to CollisionManager
 */

#include "physics/ContactListener.h"
#include "physics/CollisionManager.h"

ContactListener::ContactListener(TileMap& tileMap)
    : m_tileMap(tileMap) {
}

void ContactListener::BeginContact(b2Contact* contact) {
    CollisionManager::resolve(contact, m_tileMap);
}

void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
    (void)oldManifold;
    CollisionManager::preSolve(contact);
}
