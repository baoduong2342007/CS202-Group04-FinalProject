/**
 * @file CollisionManager.cpp
 * @author TV3
 * @brief Implementation of CollisionManager with safe pointer handling, wall-sticking prevention, and FireBall bounce resolution
 */

#include "physics/CollisionManager.h"
#include <iostream>
#include <cmath>
#include "entities/Entity.h"
#include "entities/Mario.h"
#include "entities/Enemy.h"
#include "entities/FireBall.h"
#include "physics/PhysicsEngine.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"

namespace {
constexpr float STOMP_BOUNCE_SPEED = 300.f;
constexpr float STOMP_BOUNCE_SPEED_LOW = 200.f;
constexpr float TOP_STOMP_NORMAL_THRESHOLD = 0.8f;
constexpr float BOTTOM_BLOCK_NORMAL_THRESHOLD = -0.8f;
constexpr float MAX_WALL_NORMAL_X = 0.5f;
} // namespace

void CollisionManager::resolve(b2Contact* contact) {
    if (!contact) return;

    // Zero out friction on contacts to prevent wall-sticking mid-air
    contact->SetFriction(0.0f);

    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    if (!fixtureA || !fixtureB) return;

    b2Body* bodyA = fixtureA->GetBody();
    b2Body* bodyB = fixtureB->GetBody();

    if (!bodyA || !bodyB) return;

    // Safely extract raw pointer from Box2D user data
    uintptr_t ptrA = bodyA->GetUserData().pointer;
    uintptr_t ptrB = bodyB->GetUserData().pointer;

    Entity* entityA = (ptrA != 0) ? reinterpret_cast<Entity*>(ptrA) : nullptr;
    Entity* entityB = (ptrB != 0) ? reinterpret_cast<Entity*>(ptrB) : nullptr;

    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);
    b2Vec2 normal = worldManifold.normal;

    // Handle FireBall collisions if present
    FireBall* fireBall = nullptr;
    Entity* target = nullptr;
    b2Body* fireBallBody = nullptr;

    if (entityA && (fireBall = dynamic_cast<FireBall*>(entityA))) {
        target = entityB;
        fireBallBody = bodyA;
    } else if (entityB && (fireBall = dynamic_cast<FireBall*>(entityB))) {
        target = entityA;
        fireBallBody = bodyB;
    }

    if (fireBall && fireBallBody) {
        if (!fireBall->isActive() || fireBall->isPendingDestroy()) {
            return;
        }

        if (contact->GetFixtureB()->GetBody() == fireBallBody) {
            normal = -normal; // Flip so normal points away from FireBall
        }

        if (target) {
            Enemy* enemy = dynamic_cast<Enemy*>(target);
            if (enemy) {
                enemy->takeDamage(100);
                fireBall->deactivate();
                return;
            }
        }

        // Floor contact: Bounce FireBall
        if (normal.y > 0.5f) {
            fireBall->bounce(sf::Vector2f(normal.x, normal.y));
        }
        // Wall contact: Deactivate FireBall
        else if (std::abs(normal.x) > 0.5f) {
            fireBall->deactivate();
        }
        return;
    }

    // Handle Mario collisions
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

    if (mario && marioBody) {
        handleMarioCollision(mario, otherEntity, marioBody, contact);
    }
}

void CollisionManager::handleMarioCollision(Mario* mario, Entity* other, b2Body* marioBody, b2Contact* contact) {
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);

    b2Vec2 normal = worldManifold.normal;
    if (contact->GetFixtureB()->GetBody() == marioBody) {
        normal = -normal; // Flip so normal points away from Mario
    }

    b2Vec2 marioVel = marioBody->GetLinearVelocity();

    // Top stomp / Grounded check:
    // Requires normal to be predominantly pointing UP (normal.y > 0.8f), not a steep wall (|normal.x| < 0.5f),
    // and Mario is not actively moving upward (marioVel.y >= -0.1f)
    if (normal.y > TOP_STOMP_NORMAL_THRESHOLD && std::abs(normal.x) < MAX_WALL_NORMAL_X && marioVel.y >= -0.1f) {
        mario->setGrounded(true);

        if (other) {
            Enemy* enemy = dynamic_cast<Enemy*>(other);
            if (enemy) {
                enemy->onStomp();
                EventBus::getInstance().notify(EventType::ENEMY_STOMPED);

                float currentY = marioBody->GetLinearVelocity().y;
                float bounceVel = -PhysicsEngine::pixelsToMeters(currentY > 0 ? STOMP_BOUNCE_SPEED : STOMP_BOUNCE_SPEED_LOW);
                marioBody->SetLinearVelocity(b2Vec2(marioBody->GetLinearVelocity().x, bounceVel));
            }
        }
    }
    // Bottom collision (block above Mario hit from below)
    else if (normal.y < BOTTOM_BLOCK_NORMAL_THRESHOLD) {
#ifdef DEBUG
        std::cout << "[DEBUG][CollisionManager] Mario hit overhead block from below!" << std::endl;
#endif
    }
    // Lateral collision (wall contact)
    else {
        if (other) {
            Enemy* enemy = dynamic_cast<Enemy*>(other);
            if (enemy) {
                mario->powerDown();
            }
        }
    }
}

void CollisionManager::resolveEnd(b2Contact* contact) {
    if (!contact) return;

    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();
    if (!fixtureA || !fixtureB) return;

    b2Body* bodyA = fixtureA->GetBody();
    b2Body* bodyB = fixtureB->GetBody();
    if (!bodyA || !bodyB) return;

    uintptr_t ptrA = bodyA->GetUserData().pointer;
    uintptr_t ptrB = bodyB->GetUserData().pointer;

    Entity* entityA = (ptrA != 0) ? reinterpret_cast<Entity*>(ptrA) : nullptr;
    Entity* entityB = (ptrB != 0) ? reinterpret_cast<Entity*>(ptrB) : nullptr;

    Mario* mario = nullptr;
    b2Body* marioBody = nullptr;

    if (entityA && (mario = dynamic_cast<Mario*>(entityA))) {
        marioBody = bodyA;
    } else if (entityB && (mario = dynamic_cast<Mario*>(entityB))) {
        marioBody = bodyB;
    }

    if (mario && marioBody) {
        bool stillGrounded = false;
        for (b2ContactEdge* ce = marioBody->GetContactList(); ce; ce = ce->next) {
            b2Contact* c = ce->contact;
            if (c && c->IsTouching()) {
                b2WorldManifold worldManifold;
                c->GetWorldManifold(&worldManifold);
                b2Vec2 normal = worldManifold.normal;
                if (marioBody == c->GetFixtureB()->GetBody()) {
                    normal = -normal;
                }
                b2Vec2 marioVel = marioBody->GetLinearVelocity();
                if (normal.y > TOP_STOMP_NORMAL_THRESHOLD && std::abs(normal.x) < MAX_WALL_NORMAL_X && marioVel.y >= -0.1f) {
                    stillGrounded = true;
                    break;
                }
            }
        }
        mario->setGrounded(stillGrounded);
    }
}
