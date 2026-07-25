/**
 * @file ContactListener.cpp
 * @author TV3
 * @brief ContactListener implementation for Box2D collision events
 */

#include "physics/ContactListener.h"
#include <iostream>
#include "entities/Mario.h"
#include "entities/Enemy.h"
#include "physics/PhysicsEngine.h"

namespace {
constexpr float STOMP_BOUNCE_SPEED = 300.f;
constexpr float STOMP_BOUNCE_SPEED_LOW = 200.f;
} // namespace

void ContactListener::BeginContact(b2Contact* contact) {
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    b2Body* bodyA = fixtureA->GetBody();
    b2Body* bodyB = fixtureB->GetBody();

    Entity* entityA = reinterpret_cast<Entity*>(bodyA->GetUserData().pointer);
    Entity* entityB = reinterpret_cast<Entity*>(bodyB->GetUserData().pointer);

    Mario* mario = nullptr;
    Entity* otherEntity = nullptr;
    b2Body* marioBody = nullptr;

    if (entityA && (mario = dynamic_cast<Mario*>(entityA))) {
        otherEntity = entityB;
        marioBody = bodyA;
    } else if (entityB && (mario = dynamic_cast<Mario*>(entityB))) {
        otherEntity = entityA;
        marioBody = bodyB;
    }

    // If Mario is involved in the collision
    if (mario) {
        b2WorldManifold worldManifold;
        contact->GetWorldManifold(&worldManifold);
        
        // Calculate collision normal relative to Mario
        // Box2D normal points from fixtureA to fixtureB.
        b2Vec2 normal = worldManifold.normal;
        if (marioBody == bodyB) {
            normal = -normal; // Flip so normal points from Mario to the other body
        }

        // If otherBody is below Mario (normal points downward in SFML space, so normal.y > 0.7f)
        if (normal.y > 0.7f) {
            mario->setGrounded(true);

            // Check if stomping on an enemy
            if (otherEntity) {
                Enemy* enemy = dynamic_cast<Enemy*>(otherEntity);
                if (enemy) {
                    enemy->onStomp();
                    // Make Mario bounce
                    float bounceVelocity = -PhysicsEngine::pixelsToMeters(marioBody->GetLinearVelocity().y > 0 ? STOMP_BOUNCE_SPEED : STOMP_BOUNCE_SPEED_LOW);
                    marioBody->SetLinearVelocity(b2Vec2(marioBody->GetLinearVelocity().x, bounceVelocity));
                }
            }
        }
        // If otherBody is above Mario (normal points upward in SFML space, so normal.y < -0.7f)
        else if (normal.y < -0.7f) {
            // Mario hit block from below
            // TV3/TV4 can implement block hit triggers here!
        }
        // Horizontal contact
        else {
            if (otherEntity) {
                Enemy* enemy = dynamic_cast<Enemy*>(otherEntity);
                if (enemy) {
                    mario->powerDown(); // Mario takes damage or shrinks
                }
            }
        }
    }
}

void ContactListener::EndContact(b2Contact* contact) {
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    b2Body* bodyA = fixtureA->GetBody();
    b2Body* bodyB = fixtureB->GetBody();

    Entity* entityA = reinterpret_cast<Entity*>(bodyA->GetUserData().pointer);
    Entity* entityB = reinterpret_cast<Entity*>(bodyB->GetUserData().pointer);

    Mario* mario = nullptr;
    b2Body* marioBody = nullptr;

    if (entityA && (mario = dynamic_cast<Mario*>(entityA))) {
        marioBody = bodyA;
    } else if (entityB && (mario = dynamic_cast<Mario*>(entityB))) {
        marioBody = bodyB;
    }

    if (mario) {
        // Simple grounded check: query the contact list of the body to see if any contacts are still supporting Mario from below
        bool stillGrounded = false;
        for (b2ContactEdge* ce = marioBody->GetContactList(); ce; ce = ce->next) {
            b2Contact* c = ce->contact;
            if (c->IsTouching()) {
                b2WorldManifold worldManifold;
                c->GetWorldManifold(&worldManifold);
                b2Vec2 normal = worldManifold.normal;
                if (marioBody == c->GetFixtureB()->GetBody()) {
                    normal = -normal;
                }
                if (normal.y > 0.7f) {
                    stillGrounded = true;
                    break;
                }
            }
        }
        mario->setGrounded(stillGrounded);
    }
}
