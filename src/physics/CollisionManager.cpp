/**
 * @file CollisionManager.cpp
 * @author TV3
 * @brief Implementation of CollisionManager with safe pointer handling, wall-sticking prevention, and FireBall bounce resolution
 */

#include "physics/CollisionManager.h"
#include <algorithm>
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
#include "level/TileMap.h"
#include "entities/QuestionBlock.h"
#include "physics/PhysicsEngine.h"
#include "physics/TileContactResolver.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "core/ScoreRules.h"

namespace {

constexpr float STOMP_BOUNCE_SPEED = 300.f;
constexpr float STOMP_BOUNCE_SPEED_LOW = 200.f;
constexpr float TOP_STOMP_NORMAL_THRESHOLD = 0.8f;
constexpr float BOTTOM_BLOCK_NORMAL_THRESHOLD = -0.7f;
constexpr float MAX_WALL_NORMAL_X = 0.5f;
constexpr float BLOCK_SIZE_PIXELS = 32.f;

constexpr float ENEMY_SUPPORT_PROBE_OFFSET = 2.f;
constexpr float ENEMY_WALL_NORMAL_THRESHOLD = 0.5f;
constexpr float TILE_SIZE_PIXELS = 32.f;

bool isEnemySupportObstacle(
    Entity* obstacle,
    b2Body* obstacleBody
) {
    if (!obstacleBody) {
        return false;
    }

    if (obstacle &&
        obstacle->isQuestionBlock()) {
        return true;
    }

    if (!obstacle) {
        return TileMap::isTileUserData(
            obstacleBody->GetUserData().pointer
        );
    }

    return false;
}

bool isWalkableSupportSeam(Enemy* enemy, Entity* obstacle,
                           b2Body* enemyBody, b2Body* obstacleBody,
                           const b2Vec2& enemyNormal,
                           const TileMap& tileMap
                        ) {
    if (!enemy || !enemyBody || !isEnemySupportObstacle(obstacle, obstacleBody)) {
        return false;
    }

    if (std::abs(enemyNormal.x) <= ENEMY_WALL_NORMAL_THRESHOLD) {
        return false;
    }

    const sf::Vector2f center = PhysicsEngine::metersToPixels(enemyBody->GetPosition());

    const sf::Vector2f size = enemy->getSize();

    const float bottomY = center.y + size.y / 2.f;

    const float frontX = enemyNormal.x < 0.f
                         ? center.x - size.x / 2.f - ENEMY_SUPPORT_PROBE_OFFSET
                         : center.x + size.x / 2.f + ENEMY_SUPPORT_PROBE_OFFSET;

    const int supportRow = static_cast<int>(std::floor((bottomY + ENEMY_SUPPORT_PROBE_OFFSET) / TILE_SIZE_PIXELS));

    const int frontColumn = static_cast<int>(std::floor(frontX / TILE_SIZE_PIXELS));

    if (!tileMap.isEnemySupport(frontColumn, supportRow)) {
        return false;
    }

    const bool blockedAbove = tileMap.isEnemySupport(frontColumn, supportRow - 1);

    return !blockedAbove;
}

Entity* entityFromBody(b2Body* body) {
    if (!body) {
        return nullptr;
    }

    uintptr_t ptr = body->GetUserData().pointer;
    if (ptr == 0 || TileMap::isTileUserData(ptr)) {
        return nullptr;
    }

    return reinterpret_cast<Entity*>(ptr);
}

float calculateMarioBlockOverlap(const Mario* mario,
                                 b2Body* marioBody,
                                 float blockLeft,
                                 float blockWidth) {
    if (!mario || !marioBody || blockWidth <= 0.f) {
        return 0.f;
    }

    sf::Vector2f marioPosition =
        PhysicsEngine::metersToPixels(marioBody->GetPosition()) -
        mario->getSize() / 2.0f;
    float marioLeft = marioPosition.x;
    float marioRight = marioLeft + mario->getSize().x;
    float blockRight = blockLeft + blockWidth;

    return std::max(0.f, std::min(marioRight, blockRight) - std::max(marioLeft, blockLeft));
}

void queueTileBlockHit(TileMap& tileMap, Mario* mario, b2Body* marioBody, int column, int row) {
    float overlap = calculateMarioBlockOverlap(mario,
                                               marioBody,
                                               static_cast<float>(column) * BLOCK_SIZE_PIXELS,
                                               BLOCK_SIZE_PIXELS
                                               );

    if (overlap > 0.f) {
        tileMap.queueTileHit(column, row, overlap);
    }
}

int worldToTileIndex(float coordinate) {
    return static_cast<int>(std::lround(coordinate / BLOCK_SIZE_PIXELS));
}

void queueTileBlockHitFromContact(TileMap& tileMap,
                                  Mario* mario,
                                  b2Body* marioBody,
                                  b2Contact* contact,
                                  const b2WorldManifold& worldManifold
                                  ) {
    if (!mario || !marioBody || !contact) {
        return;
    }

    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();
    b2Fixture* tileFixture = (fixtureA && fixtureA->GetBody() == marioBody) ? fixtureB : fixtureA;
    if (!tileFixture) {
        return;
    }

    // A merged span is a single body. Clamp a boundary contact into that span
    // before resolving its individual tile, so an adjacent empty tile is never queued.
    const b2AABB spanBounds = tileFixture->GetAABB(0);
    const float spanMinX = PhysicsEngine::metersToPixels(spanBounds.lowerBound.x);
    const float spanMaxX = PhysicsEngine::metersToPixels(spanBounds.upperBound.x);
    const float spanMinY = PhysicsEngine::metersToPixels(spanBounds.lowerBound.y);
    const float spanMaxY = PhysicsEngine::metersToPixels(spanBounds.upperBound.y);
    // Box2D expands fixture AABBs by its polygon skin. One pixel is enough
    // to move a boundary point into the actual tile span before the resolver
    // applies its own contact-edge bias.
    constexpr float SPAN_EDGE_EPSILON_PIXELS = 1.0f;

    const int pointCount = contact->GetManifold()->pointCount;
    for (int pointIndex = 0; pointIndex < pointCount; ++pointIndex) {
        const sf::Vector2f contactPointPixels =
            PhysicsEngine::metersToPixels(worldManifold.points[pointIndex]);
        const float clampedX = std::clamp(contactPointPixels.x,
                                          spanMinX + SPAN_EDGE_EPSILON_PIXELS,
                                          spanMaxX - SPAN_EDGE_EPSILON_PIXELS);
        const float clampedY = std::clamp(contactPointPixels.y,
                                          spanMinY + SPAN_EDGE_EPSILON_PIXELS,
                                          spanMaxY - SPAN_EDGE_EPSILON_PIXELS);
        const TileContactCoordinate tile = resolveCeilingTileContact(
            clampedX,
            clampedY,
            BLOCK_SIZE_PIXELS
        );
        queueTileBlockHit(
            tileMap,
            mario,
            marioBody,
            tile.column,
            tile.row
        );
    }
}

void queueEntityBlockHit(TileMap& tileMap, Mario* mario, b2Body* marioBody, const Entity& block) {
    sf::Vector2f position = block.getPosition();

    float overlap = calculateMarioBlockOverlap(mario,
                                               marioBody,
                                               position.x,
                                               block.getSize().x
                                               );

    if (overlap > 0.f) {
        tileMap.queueTileHit(worldToTileIndex(position.x),
                             worldToTileIndex(position.y),
                             overlap
                             );
    }
}

void handleEnemyWallCollision(Enemy* enemy, Entity* obstacle,
                              b2Body* enemyBody, b2Body* obstacleBody,
                              const b2Vec2& enemyNormal,
                              const TileMap& tileMap
                              ) {
    if (!enemy || !enemyBody || enemy->isDying()) {
        return;
    }

    if (std::abs(enemyNormal.x) <= ENEMY_WALL_NORMAL_THRESHOLD) {
        return;
    }

    if (isWalkableSupportSeam(enemy, obstacle,
                               enemyBody, obstacleBody,
                               enemyNormal,
                               tileMap)) {
        return;
    }

    const Direction wallDirection = enemyNormal.x < 0.f
                                    ? Direction::LEFT : Direction::RIGHT;

    if (enemy->getFacingDirection() != wallDirection) {
        return;
    }

    enemy->onWallCollision();
}

} // namespace

bool CollisionManager::defeatEnemy(Enemy& victim,
                                    DefeatCause cause,
                                    Mario* owner) {
    if (cause == DefeatCause::STOMP) {
        // A Koopa stomp is an interaction rather than a terminal death: the
        // walking/sliding shell changes state and remains available for a
        // later shell-kill transaction. Goomba stomp still transitions the
        // victim to its terminal squish state.
        if (!victim.tryCommitStomp()) {
            return false;
        }

        victim.onStomp();
        if (owner) {
            owner->addScore(ScoreRules::pointsFor(cause));
        }
        EventBus::getInstance().notify(EventType::ENEMY_STOMPED);
        return true;
    }

    if (!victim.tryCommitDefeat()) {
        return false;
    }

    switch (cause) {
        case DefeatCause::SHELL:
            victim.takeDamage(victim.getHealth());
            victim.markForRemoval();
            EventBus::getInstance().notify(EventType::ENEMY_DEFEATED_BY_SHELL);
            break;
        case DefeatCause::FIREBALL:
            victim.onFireHit();
            EventBus::getInstance().notify(EventType::ENEMY_DEFEATED_BY_FIREBALL);
            break;
        case DefeatCause::STAR:
            victim.takeDamage(victim.getHealth());
            victim.markForRemoval();
            EventBus::getInstance().notify(EventType::ENEMY_DEFEATED_BY_STAR);
            break;
        case DefeatCause::PIT:
            victim.takeDamage(victim.getHealth());
            victim.markForRemoval();
            break;
        case DefeatCause::STOMP:
            // Handled above; keep the switch exhaustive for future callers.
            return false;
    }

    if (owner) {
        owner->addScore(ScoreRules::pointsFor(cause));
    }
    return true;
}

void CollisionManager::preSolve(b2Contact* contact, TileMap& tileMap) {
    if (!contact) {
        return;
    }

    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    if (!fixtureA || !fixtureB) {
        return;
    }

    Entity* entityA = entityFromBody(fixtureA->GetBody());
    Entity* entityB = entityFromBody(fixtureB->GetBody());

    if (entityA && entityB &&
        entityA->isEnemy() && entityB->isEnemy()) {
        contact->SetEnabled(false);
        return;
    }

    if ((entityA && entityA->isMario()) || (entityB && entityB->isMario())) {
        contact->SetFriction(0.0f);
    }

    if (entityA && entityA->isEnemy() && static_cast<Enemy*>(entityA)->isDying()) {
        contact->SetEnabled(false);
    }
    if (entityB && entityB->isEnemy() && static_cast<Enemy*>(entityB)->isDying()) {
        contact->SetEnabled(false);
    }

    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);

    if (entityA && entityA->isEnemy() &&
        isWalkableSupportSeam(static_cast<Enemy*>(entityA), entityB,
                              fixtureA->GetBody(), fixtureB->GetBody(),
                              worldManifold.normal,
                              tileMap
                            )) {

        contact->SetEnabled(false);
        return;
    }

    if (entityB && entityB->isEnemy() &&
        isWalkableSupportSeam(static_cast<Enemy*>(entityB), entityA,
                              fixtureB->GetBody(), fixtureA->GetBody(),
                              -worldManifold.normal,
                              tileMap)) {
        contact->SetEnabled(false);
        return;
    }
}

void CollisionManager::resolve(b2Contact* contact, TileMap& tileMap) {
    if (!contact) return;

    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    if (!fixtureA || !fixtureB) return;

    b2Body* bodyA = fixtureA->GetBody();
    b2Body* bodyB = fixtureB->GetBody();

    if (!bodyA || !bodyB) return;

    // Safely extract raw pointer from Box2D user data
    Entity* entityA = entityFromBody(bodyA);
    Entity* entityB = entityFromBody(bodyB);

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

        // Ignore collisions between FireBall and Mario
        if (target && target->isMario()) {
            return;
        }

        if (target) {
            if (target->isEnemy()) {
                Enemy* enemy = static_cast<Enemy*>(target);
                defeatEnemy(*enemy, DefeatCause::FIREBALL, fireBall->getOwner());
                fireBall->deactivate();
                return;
            }
        }

        // Floor contact: Bounce FireBall
        if (normal.y > 0.5f) {
            fireBall->bounce(sf::Vector2f(normal.x, normal.y));
        }
        // Wall contact: Deactivate FireBall (require small lifetime grace period so initial spawn doesn't instantly die)
        else if (std::abs(normal.x) > 0.5f) {
            if (fireBall->getLifetime() > 0.05f) {
                fireBall->deactivate();
            }
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
        // Item pickup is resolved here, alongside every other Mario gameplay
        // collision. Level's overlap sweep remains a fallback for items whose
        // sensor contact was not reported by Box2D, while Item::isCollectible()
        // makes the operation idempotent.
        if (otherEntity && otherEntity->isItem()) {
            Item* item = static_cast<Item*>(otherEntity);
            if (item->isCollectible()) {
                item->onCollect(*mario);
                item->markForRemoval();
            }
            return;
        }
        handleMarioCollision(mario, otherEntity, marioBody, contact, tileMap);
        return;
    }

    // Handle collisions between enemies (TV5 - Koopa shell kills Goomba/Enemies)
    if (entityA && entityA->isEnemy() && entityB && entityB->isEnemy()) {
        Enemy* enemyA = static_cast<Enemy*>(entityA);
        Enemy* enemyB = static_cast<Enemy*>(entityB);

        if (enemyA->isDying() || enemyB->isDying()) {
            return;
        }

        auto tryShellKill = [](Enemy* attacker, Enemy* victim) -> bool {
            if (!attacker || !victim) {
                return false;
            }

            if (!attacker->isKoopa() || victim->isKoopa()) {
                return false;
            }

            Koopa* koopa = static_cast<Koopa*>(attacker);

            if (!koopa->isShellSliding()) {
                return false;
            }

            return CollisionManager::defeatEnemy(
                *victim,
                DefeatCause::SHELL,
                koopa->getDefeatOwner());
        };

        tryShellKill(enemyA, enemyB);
        tryShellKill(enemyB, enemyA);

        return;
    }

    // Handle Enemy ↔ Wall / Static Body collisions (Task 3.1)
    if (entityA && entityA->isEnemy()) {
        handleEnemyWallCollision(static_cast<Enemy*>(entityA), entityB,
                                 bodyA, bodyB,
                                 normal,
                                 tileMap
                                 );
    } else if (entityB && entityB->isEnemy()) {
        handleEnemyWallCollision(static_cast<Enemy*>(entityB), entityA,
                                 bodyB, bodyA,
                                 -normal,
                                 tileMap
                                 );
    }

    // Handle Item ↔ Wall / Static Body collisions
    if (entityA && entityA->isItem()) {
        b2Vec2 itemNormal = normal;
        if (entityA->isMushroom()) {
            Mushroom* mushroom = static_cast<Mushroom*>(entityA);
            if (std::abs(itemNormal.x) > 0.1f) mushroom->onWallCollision();
        } else if (entityA->isStar()) {
            Star* star = static_cast<Star*>(entityA);
            if (std::abs(itemNormal.x) > 0.1f) star->onWallCollision();
            if (itemNormal.y > 0.8f) star->onGroundCollision();
        }
    } else if (entityB && entityB->isItem()) {
        b2Vec2 itemNormal = -normal;
        if (entityB->isMushroom()) {
            Mushroom* mushroom = static_cast<Mushroom*>(entityB);
            if (std::abs(itemNormal.x) > 0.1f) mushroom->onWallCollision();
        } else if (entityB->isStar()) {
            Star* star = static_cast<Star*>(entityB);
            if (std::abs(itemNormal.x) > 0.1f) star->onWallCollision();
            if (itemNormal.y > 0.8f) star->onGroundCollision();
        }
    }
}

void CollisionManager::handleMarioCollision(Mario* mario,
                                            Entity* other,
                                            b2Body* marioBody,
                                            b2Contact* contact,
                                            TileMap& tileMap
                                            ) {
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);

    b2Vec2 normal = worldManifold.normal;
    if (contact->GetFixtureB()->GetBody() == marioBody) {
        normal = -normal; // Flip so normal points away from Mario
    }

    b2Vec2 marioVel = marioBody->GetLinearVelocity();

    if (!other) {
        // Head bump check against TileMap blocks from below
        b2Body* otherBody = (contact->GetFixtureA()->GetBody() == marioBody)
                            ? contact->GetFixtureB()->GetBody()
                            : contact->GetFixtureA()->GetBody();
        const uintptr_t otherPtr = otherBody ? otherBody->GetUserData().pointer : 0;

        if (TileMap::isTileUserData(otherPtr)) {
            if (normal.y < BOTTOM_BLOCK_NORMAL_THRESHOLD && marioVel.y < -0.1f) {
                queueTileBlockHitFromContact(tileMap, mario, marioBody, contact, worldManifold);
            }
        }
    }

    // Top stomp check. Grounded state itself is refreshed from all active
    // Box2D contacts after each completed physics step.
    bool isStomp = false;
    if (other && other->isEnemy()) {
        Enemy* enemy = static_cast<Enemy*>(other);
        if (enemy->isDying()) {
            return;
        }

        // Star invincibility is a separate gameplay authority from damage
        // grace. It defeats the enemy through the same cause/score/event
        // transaction regardless of whether the contact looks like a stomp
        // or a side hit.
        if (mario->isStarInvincible()) {
            CollisionManager::defeatEnemy(*enemy, DefeatCause::STAR, mario);
            mario->clearGroundedState();
            return;
        }

        if (normal.y > TOP_STOMP_NORMAL_THRESHOLD && std::abs(normal.x) < MAX_WALL_NORMAL_X) {
            isStomp = true;
        } else {
            b2Body* otherBody = other->getBody();
            if (otherBody && marioBody) {
                float marioHalfHeight = PhysicsEngine::pixelsToMeters(mario->getSize().y / 2.0f);
                float marioBottomMeters = marioBody->GetPosition().y + marioHalfHeight;
                float enemyMidMeters = otherBody->GetPosition().y;
                float tolerance = PhysicsEngine::pixelsToMeters(other->getSize().y * 0.2f);
                if (marioBottomMeters <= enemyMidMeters + tolerance) {
                    isStomp = true;
                }
            }
        }
    }

    if (isStomp) {
        Enemy* enemy = static_cast<Enemy*>(other);

        if (!CollisionManager::defeatEnemy(*enemy, DefeatCause::STOMP, mario)) {
            return;
        }

        float currentY = marioBody->GetLinearVelocity().y;

        float bounceVel = -PhysicsEngine::pixelsToMeters(currentY > 0
                                                         ? STOMP_BOUNCE_SPEED : STOMP_BOUNCE_SPEED_LOW
                                                         );

        marioBody->SetLinearVelocity(b2Vec2(marioBody->GetLinearVelocity().x, bounceVel));

        mario->clearGroundedState();
        return;
    }
    // Bottom collision (block above Mario hit from below)
    else if (normal.y < BOTTOM_BLOCK_NORMAL_THRESHOLD && marioVel.y < -0.1f) {
        if (other && other->isQuestionBlock()) {
            queueEntityBlockHit(tileMap, mario, marioBody, *other);
        }
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
                        mario->queuePowerDown();
                    }
                    // If shell is idle, Mario kicks it
                    else if (koopa->isInShell()) {
                        Direction kickDir = (mario->getPosition().x < koopa->getPosition().x) ? Direction::RIGHT : Direction::LEFT;
                        koopa->setDefeatOwner(mario);
                        koopa->kick(kickDir);
                    }
                    // If walking, Mario gets hit
                    else {
                        mario->queuePowerDown();
                    }
                } else {
                    mario->queuePowerDown();
                }
            }
        }
    }

    // Terrain contact is also resolved centrally now that entity callbacks no
    // longer perform gameplay decisions.
    if ((!other || !other->isEnemy()) &&
        normal.y > TOP_STOMP_NORMAL_THRESHOLD &&
        std::abs(normal.x) < MAX_WALL_NORMAL_X &&
        marioVel.y >= -0.1f) {
        mario->setGrounded(true);
    }
}
