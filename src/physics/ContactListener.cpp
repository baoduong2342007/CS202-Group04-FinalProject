/**
 * @file ContactListener.cpp
 * @author TV3
 * @brief ContactListener implementation delegating to CollisionManager
 */

#include "physics/ContactListener.h"
#include "physics/CollisionManager.h"

void ContactListener::BeginContact(b2Contact* contact) {
    CollisionManager::resolve(contact);
}

void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
    (void)oldManifold;
    CollisionManager::preSolve(contact);
}
