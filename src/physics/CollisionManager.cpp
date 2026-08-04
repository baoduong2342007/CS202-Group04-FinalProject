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
#include "entities/Koopa.h"
#include "entities/FireBall.h"
#include "items/Item.h"
#include "items/Mushroom.h"
#include "items/Star.h"
#include "physics/PhysicsEngine.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"

namespace {
constexpr float STOMP_BOUNCE_SPEED = 300.f;
constexpr float STOMP_BOUNCE_SPEED_LOW = 200.f;
constexpr float TOP_STOMP_NORMAL_THRESHOLD = 0.8f;
constexpr float BOTTOM_BLOCK_NORMAL_THRESHOLD = -0.7f;
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

    if (entityA && entityA->isFireBall()) {
        fireBall = static_cast<FireBall*>(entityA);
        target = entityB;
        fireBallBody = bodyA;
    } else if (entityB && entityB->isFireBall()) {
        fireBall = static_cast<FireBall*>(entityB);
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
            if (target->isEnemy()) {
                Enemy* enemy = static_cast<Enemy*>(target);
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

    if (entityA && entityA->isMario()) {
        mario = static_cast<Mario*>(entityA);
        otherEntity = entityB;
        marioBody = bodyA;
    } else if (entityB && entityB->isMario()) {
        mario = static_cast<Mario*>(entityB);
        otherEntity = entityA;
        marioBody = bodyB;
    }

    if (mario && marioBody) {
        handleMarioCollision(mario, otherEntity, marioBody, contact);
        return;
    }

    // Handle collisions between enemies
    if (entityA && entityA->isEnemy() && entityB && entityB->isEnemy()) {
        Enemy* enemyA = static_cast<Enemy*>(entityA);
        Enemy* enemyB = static_cast<Enemy*>(entityB);

        auto tryShellKill = [](Enemy* attacker, Enemy* victim) -> bool {
            if (!attacker->isKoopa() || victim->isKoopa()) {
                return false;
            }

            Koopa* koopa = static_cast<Koopa*>(attacker);

            if (!koopa->isShellSliding()) {
                return false;
            }

            victim->takeDamage(100);
            victim->markForRemoval();
            return true;
        };

        const bool shellCollisionHandled = tryShellKill(enemyA, enemyB) || tryShellKill(enemyB, enemyA);

        // Ordinary enemies reverse when they collide laterally.
        if (!shellCollisionHandled && std::abs(normal.x) > 0.5f) {
            enemyA->onWallCollision();
            enemyB->onWallCollision();
        }

        return;
    }
    
    // Handle Enemy ↔ Wall / Static Body collisions (Task 3.1)
    Enemy* enemy = nullptr;
    if (entityA && entityA->isEnemy()) {
        enemy = static_cast<Enemy*>(entityA);
        if (std::abs(normal.x) > 0.5f) {
            enemy->onWallCollision();
        }
    } else if (entityB && entityB->isEnemy()) {
        enemy = static_cast<Enemy*>(entityB);
        if (std::abs(normal.x) > 0.5f) {
            enemy->onWallCollision();
        }
    }

    // Handle Item ↔ Wall / Static Body collisions
    if (entityA && entityA->isItem()) {
        b2Vec2 itemNormal = normal;
        if (entityA->isMushroom()) {
            Mushroom* mushroom = static_cast<Mushroom*>(entityA);
            if (std::abs(itemNormal.x) > 0.5f) mushroom->onWallCollision();
        } else if (entityA->isStar()) {
            Star* star = static_cast<Star*>(entityA);
            if (std::abs(itemNormal.x) > 0.5f) star->onWallCollision();
            if (itemNormal.y > 0.8f) star->onGroundCollision();
        }
    } else if (entityB && entityB->isItem()) {
        b2Vec2 itemNormal = -normal;
        if (entityB->isMushroom()) {
            Mushroom* mushroom = static_cast<Mushroom*>(entityB);
            if (std::abs(itemNormal.x) > 0.5f) mushroom->onWallCollision();
        } else if (entityB->isStar()) {
            Star* star = static_cast<Star*>(entityB);
            if (std::abs(itemNormal.x) > 0.5f) star->onWallCollision();
            if (itemNormal.y > 0.8f) star->onGroundCollision();
        }
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

    // Check collectible Item collection (Task 3.1)
    if (other) {
        if (other->isItem()) {
            Item* item = static_cast<Item*>(other);
            if (!item->isCollected()) {
                item->onCollect(*mario);
            }
        }
    }

    // Top stomp / Grounded check:
    // Requires normal to be predominantly pointing UP (normal.y > 0.8f), not a steep wall (|normal.x| < 0.5f),
    // and Mario is not actively moving upward (marioVel.y >= -0.1f)
    if (normal.y > TOP_STOMP_NORMAL_THRESHOLD && std::abs(normal.x) < MAX_WALL_NORMAL_X && marioVel.y >= -0.1f) {
        mario->setGrounded(true);

        if (other) {
            if (other->isEnemy()) {
                Enemy* enemy = static_cast<Enemy*>(other);

                // Koopa Kick Logic: If Koopa is in shell idle, kick it. If sliding, take damage.
                if (enemy->isKoopa()) {
                    Koopa* koopa = static_cast<Koopa*>(enemy);
                    if (koopa->isInShell() && !koopa->isShellSliding()) {
                        Direction kickDir = (mario->getPosition().x < koopa->getPosition().x) ? Direction::RIGHT : Direction::LEFT;
                        koopa->kick(kickDir);
                        EventBus::getInstance().notify(EventType::ENEMY_STOMPED);

                        float currentY = marioBody->GetLinearVelocity().y;
                        float bounceVel = -PhysicsEngine::pixelsToMeters(currentY > 0 ? STOMP_BOUNCE_SPEED : STOMP_BOUNCE_SPEED_LOW);
                        marioBody->SetLinearVelocity(b2Vec2(marioBody->GetLinearVelocity().x, bounceVel));
                        return;
                    }
                }

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
            if (other->isEnemy()) {
                Enemy* enemy = static_cast<Enemy*>(other);
                
                // Koopa Kick Logic
                if (enemy->isKoopa()) {
                    Koopa* koopa = static_cast<Koopa*>(enemy);
                    // If shell is sliding, Mario gets hit
                    if (koopa->isShellSliding()) {
                        mario->powerDown();
                    } 
                    // If shell is idle, Mario kicks it
                    else if (koopa->isInShell()) {
                        Direction kickDir = (mario->getPosition().x < koopa->getPosition().x) ? Direction::RIGHT : Direction::LEFT;
                        koopa->kick(kickDir);
                        EventBus::getInstance().notify(EventType::ENEMY_STOMPED);
                    } 
                    // If walking, Mario gets hit
                    else {
                        mario->powerDown();
                    }
                } else {
                    mario->powerDown();
                }
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

    if (entityA && entityA->isMario()) {
        mario = static_cast<Mario*>(entityA);
        marioBody = bodyA;
    } else if (entityB && entityB->isMario()) {
        mario = static_cast<Mario*>(entityB);
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
