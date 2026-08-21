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
#include "entities/CheepCheep.h"
#include "entities/FireBall.h"
#include "items/Item.h"
#include "items/Mushroom.h"
#include "items/Star.h"
#include "level/TileMap.h"
#include "entities/QuestionBlock.h"
#include "entities/Springboard.h"
#include "physics/PhysicsEngine.h"
#include "physics/TileContactResolver.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "core/ScoreRules.h"

std::vector<PvpHit> CollisionManager::s_pendingPvpHits;

CollisionParticipant::CollisionParticipant(Entity* entity, b2Body* body) noexcept
    : m_entity(entity), m_body(body) {}

Entity::EntityType CollisionParticipant::type() const noexcept {
    return m_entity ? m_entity->getType() : Entity::EntityType::UNKNOWN;
}

Entity::EntitySubtype CollisionParticipant::subtype() const noexcept {
    return m_entity ? m_entity->getSubtype() : Entity::EntitySubtype::UNKNOWN;
}

bool CollisionParticipant::has(Entity::Capability capability) const noexcept {
    return m_entity && m_entity->hasCapability(capability);
}

Mario* CollisionParticipant::mario() const noexcept {
    return type() == Entity::EntityType::MARIO ? dynamic_cast<Mario*>(m_entity) : nullptr;
}

Enemy* CollisionParticipant::enemy() const noexcept {
    return type() == Entity::EntityType::ENEMY ? dynamic_cast<Enemy*>(m_entity) : nullptr;
}

FireBall* CollisionParticipant::fireBall() const noexcept {
    return subtype() == Entity::EntitySubtype::FIRE_BALL
        ? dynamic_cast<FireBall*>(m_entity) : nullptr;
}

Item* CollisionParticipant::item() const noexcept {
    return type() == Entity::EntityType::ITEM && has(Entity::Capability::COLLECTIBLE)
        ? dynamic_cast<Item*>(m_entity) : nullptr;
}

Koopa* CollisionParticipant::shell() const noexcept {
    return has(Entity::Capability::SHELL_LIKE) ? dynamic_cast<Koopa*>(m_entity) : nullptr;
}

Mushroom* CollisionParticipant::mushroom() const noexcept {
    return subtype() == Entity::EntitySubtype::MUSHROOM
        ? dynamic_cast<Mushroom*>(m_entity) : nullptr;
}

Star* CollisionParticipant::star() const noexcept {
    return subtype() == Entity::EntitySubtype::STAR
        ? dynamic_cast<Star*>(m_entity) : nullptr;
}

Springboard* CollisionParticipant::springboard() const noexcept {
    return subtype() == Entity::EntitySubtype::SPRINGBOARD
        ? dynamic_cast<Springboard*>(m_entity) : nullptr;
}

CollisionContext::CollisionContext(CollisionParticipant first,
                                   CollisionParticipant second,
                                   b2Contact* contact,
                                   const b2Vec2& normal) noexcept
    : m_first(first), m_second(second), m_contact(contact), m_normal(normal) {}

b2Vec2 CollisionContext::normalFrom(
    const CollisionParticipant& participant) const noexcept {
    return &participant == &m_second ? -m_normal : m_normal;
}

const CollisionParticipant* CollisionContext::other(
    const CollisionParticipant& participant) const noexcept {
    if (&participant == &m_first) return &m_second;
    if (&participant == &m_second) return &m_first;
    return nullptr;
}

void CollisionManager::queuePvpHit(const PvpHit& hit) {
    s_pendingPvpHits.push_back(hit);
}

std::vector<PvpHit> CollisionManager::consumePendingPvpHits() {
    std::vector<PvpHit> drained;
    drained.swap(s_pendingPvpHits);
    return drained;
}

void CollisionManager::clearPendingPvpHits() {
    s_pendingPvpHits.clear();
}

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
constexpr float ENEMY_FALLING_VELOCITY_THRESHOLD = 0.01f;
constexpr float MARIO_SEAM_MIN_HORIZONTAL_NORMAL = 0.2f;
constexpr float MARIO_SEAM_MIN_UPWARD_NORMAL = 0.2f;
constexpr float MARIO_SEAM_TOP_TOLERANCE_PIXELS = 3.f;
constexpr float MARIO_SEAM_FALLING_VELOCITY_THRESHOLD = 0.01f;

bool isTerrainOrQuestionBlock(
    Entity* obstacle,
    b2Body* obstacleBody
) {
    if (!obstacleBody) {
        return false;
    }

    if (obstacle && obstacle->getSubtype() == Entity::EntitySubtype::QUESTION_BLOCK) {
        return true;
    }

    if (!obstacle) {
        return TileMap::isTileUserData(
            obstacleBody->GetUserData().pointer
        );
    }

    return false;
}

// Post-hit invulnerability in the original game is also a collision phase:
// harmful enemies are allowed to pass through Mario instead of remaining
// solid and pushing him while the sprite is blinking. An idle Koopa shell is
// intentionally kept interactive so Mario can still kick it. Star power is
// kept separate because its contact must still defeat the enemy.
bool isDamageDealingEnemy(Entity* entity) {
    if (!entity || entity->getType() != Entity::EntityType::ENEMY) {
        return false;
    }

    if (!entity->hasCapability(Entity::Capability::SHELL_LIKE)) {
        return true;
    }

    const Koopa* koopa = dynamic_cast<const Koopa*>(entity);
    if (!koopa) return false;
    return !koopa->isInShell() || koopa->isShellSliding();
}

bool isDamageGraceEnemyContact(Entity* entityA, Entity* entityB) {
    Mario* mario = nullptr;
    Entity* other = nullptr;

    if (entityA && entityA->getType() == Entity::EntityType::MARIO) {
        mario = dynamic_cast<Mario*>(entityA);
        other = entityB;
    } else if (entityB && entityB->getType() == Entity::EntityType::MARIO) {
        mario = dynamic_cast<Mario*>(entityB);
        other = entityA;
    }

    return mario && isDamageDealingEnemy(other) && mario->isDamageImmune() &&
           !mario->isStarInvincible();
}

bool isWalkableSupportSeam(Enemy* enemy, Entity* obstacle,
                           b2Body* enemyBody, b2Body* obstacleBody,
                           const b2Vec2& enemyNormal,
                           const TileMap& tileMap
                        ) {
    if (!enemy || !enemyBody || !isTerrainOrQuestionBlock(obstacle, obstacleBody)) {
        return false;
    }

    if (std::abs(enemyNormal.x) <= ENEMY_WALL_NORMAL_THRESHOLD) {
        return false;
    }

    // A side contact can look like a one-tile step when the enemy is falling
    // beside a pit wall. Disabling that contact lets the lower corner of the
    // next tile catch the enemy and leaves it stuck against the wall. Seam
    // suppression is only valid while the enemy is travelling horizontally
    // along the top of the terrain.
    if (enemyBody->GetLinearVelocity().y > ENEMY_FALLING_VELOCITY_THRESHOLD) {
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

    int requiredClearanceRows = 1;

    // Walking Koopa is 48px tall, so it needs
    // two tile rows of vertical clearance.
    if (enemy->hasCapability(Entity::Capability::SHELL_LIKE)) {
        const auto* koopa = dynamic_cast<const Koopa*>(enemy);
        if (!koopa) return false;

        if (!koopa->isInShell()) {
            requiredClearanceRows = 2;
        }
    }

    for (int offset = 1;
         offset <= requiredClearanceRows;
         ++offset) {

        if (tileMap.isEnemySupport(frontColumn, supportRow - offset)) {
            return false;
        }
    }

    return true;
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

b2Vec2 normalFromMarioTowardsObstacle(const b2Contact& contact,
                                       const b2Body* marioBody,
                                       const b2WorldManifold& manifold) {
    b2Vec2 normal = manifold.normal;
    if (contact.GetFixtureB()->GetBody() == marioBody) {
        normal = -normal;
    }
    return normal;
}

float fixtureTopPixels(const b2Fixture& fixture) {
    return PhysicsEngine::metersToPixels(fixture.GetAABB(0).lowerBound.y);
}

float fixtureBottomPixels(const b2Fixture& fixture) {
    return PhysicsEngine::metersToPixels(fixture.GetAABB(0).upperBound.y);
}

bool hasSameHeightMarioSupport(Mario* mario,
                               b2Body* marioBody,
                               b2Body* seamObstacleBody,
                               float seamTopPixels) {
    if (!mario || !marioBody) {
        return false;
    }

    for (b2ContactEdge* edge = marioBody->GetContactList(); edge;
         edge = edge->next) {
        b2Contact* supportContact = edge->contact;
        if (!supportContact || !supportContact->IsTouching() ||
            edge->other == seamObstacleBody) {
            continue;
        }

        b2Fixture* fixtureA = supportContact->GetFixtureA();
        b2Fixture* fixtureB = supportContact->GetFixtureB();
        if (!fixtureA || !fixtureB) {
            continue;
        }

        b2Fixture* supportFixture =
            fixtureA->GetBody() == marioBody ? fixtureB : fixtureA;
        if (!supportFixture ||
            !isTerrainOrQuestionBlock(entityFromBody(edge->other), edge->other)) {
            continue;
        }

        b2WorldManifold supportManifold;
        supportContact->GetWorldManifold(&supportManifold);
        const b2Vec2 supportNormal = normalFromMarioTowardsObstacle(
            *supportContact, marioBody, supportManifold);
        if (supportNormal.y < TOP_STOMP_NORMAL_THRESHOLD ||
            std::abs(supportNormal.x) >= MAX_WALL_NORMAL_X) {
            continue;
        }

        if (std::abs(fixtureTopPixels(*supportFixture) - seamTopPixels) <=
            MARIO_SEAM_TOP_TOLERANCE_PIXELS) {
            return true;
        }
    }

    return false;
}

bool isMarioWalkableSupportSeam(Mario* mario,
                                Entity* obstacle,
                                b2Body* marioBody,
                                b2Body* obstacleBody,
                                b2Contact* contact,
                                const b2WorldManifold& manifold) {
    if (!mario || !marioBody || !obstacleBody || !contact ||
        !isTerrainOrQuestionBlock(obstacle, obstacleBody)) {
        return false;
    }

    const b2Vec2 normal =
        normalFromMarioTowardsObstacle(*contact, marioBody, manifold);
    if (std::abs(normal.x) < MARIO_SEAM_MIN_HORIZONTAL_NORMAL ||
        normal.y < MARIO_SEAM_MIN_UPWARD_NORMAL ||
        marioBody->GetLinearVelocity().y > MARIO_SEAM_FALLING_VELOCITY_THRESHOLD) {
        return false;
    }

    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();
    if (!fixtureA || !fixtureB) {
        return false;
    }

    b2Fixture* marioFixture =
        fixtureA->GetBody() == marioBody ? fixtureA : fixtureB;
    b2Fixture* obstacleFixture =
        fixtureA->GetBody() == marioBody ? fixtureB : fixtureA;
    if (!marioFixture || !obstacleFixture) {
        return false;
    }

    const float obstacleTopPixels = fixtureTopPixels(*obstacleFixture);
    if (std::abs(fixtureBottomPixels(*marioFixture) - obstacleTopPixels) >
        MARIO_SEAM_TOP_TOLERANCE_PIXELS) {
        return false;
    }

    return hasSameHeightMarioSupport(mario, marioBody, obstacleBody,
                                     obstacleTopPixels);
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
                                    Mario* owner,
                                    int streakIndex) {
    // Indestructible enemies (Podoboo) survive every defeat cause; callers
    // treat the false return as "no transaction happened".
    if (victim.isIndestructible() && cause != DefeatCause::PIT) {
        return false;
    }

    if (cause == DefeatCause::STOMP) {
        // A Koopa stomp is an interaction rather than a terminal death: the
        // walking/sliding shell changes state and remains available for a
        // later shell-kill transaction. Goomba stomp still transitions the
        // victim to its terminal squish state.
        if (!victim.tryCommitStomp()) {
            return false;
        }

        const sf::Vector2f popupPosition =
            victim.getPosition() + sf::Vector2f(victim.getSize().x / 2.f, 0.f);
        victim.onStomp();
        if (owner) {
            const int speciesStompScore = victim.getStompScore();
            if (speciesStompScore > 0) {
                // Species with a flat stomp price (Bullet Bill, Lakitu,
                // Hammer Bro) never feed the airborne stomp chain.
                owner->queueScoreAward(popupPosition, speciesStompScore, false);
            } else {
                owner->awardStompScore(popupPosition);
            }
        }
        EventBus::getInstance().notify(EventType::ENEMY_STOMPED);
        return true;
    }

    if (!victim.tryCommitDefeat()) {
        return false;
    }

    const sf::Vector2f popupPosition =
        victim.getPosition() + sf::Vector2f(victim.getSize().x / 2.f, 0.f);

    int points = 0;
    bool grantsLife = false;

    switch (cause) {
        case DefeatCause::SHELL: {
            // Shell defeats use the same launched, flipped presentation as a
            // FireBall. The defeat transaction still owns the shell-specific
            // score/event; the enemy owns its death animation and cleanup.
            victim.onFireHit();
            EventBus::getInstance().notify(EventType::ENEMY_DEFEATED_BY_SHELL);
            constexpr std::array<int, 8> SHELL_SCORE_CHAIN = {
                200, 400, 800, 1000, 2000, 4000, 5000, 8000
            };
            if (streakIndex >= static_cast<int>(SHELL_SCORE_CHAIN.size())) {
                grantsLife = true;
                points = 0;
            } else {
                points = SHELL_SCORE_CHAIN[std::max(0, streakIndex)];
            }
            break;
        }
        case DefeatCause::FIREBALL:
            victim.onFireHit();
            EventBus::getInstance().notify(EventType::ENEMY_DEFEATED_BY_FIREBALL);
            points = ScoreRules::pointsFor(DefeatCause::FIREBALL);
            break;
        case DefeatCause::STAR:
            // Star contact uses the fireball-style flipped death for most
            // enemies; multi-hit bosses override onStarHit to die outright.
            // The transaction latch above still owns score/event
            // deduplication, while the enemy owns its presentation/lifecycle.
            victim.onStarHit();
            EventBus::getInstance().notify(EventType::ENEMY_DEFEATED_BY_STAR);
            points = ScoreRules::pointsFor(DefeatCause::STAR);
            break;
        case DefeatCause::BLOCK_BUMP:
            // A block bump uses the same launched death presentation as a
            // FireBall while keeping its own score/event identity.
            victim.onFireHit();
            EventBus::getInstance().notify(EventType::ENEMY_DEFEATED_BY_BLOCK);
            points = ScoreRules::pointsFor(DefeatCause::BLOCK_BUMP);
            break;
        case DefeatCause::PIT:
            victim.takeDamage(victim.getHealth());
            victim.markForRemoval();
            return true;
        case DefeatCause::STOMP:
            // Handled above; keep the switch exhaustive for future callers.
            return false;
    }

    // Species-specific prices (Hammer Bro's flat 1000, Bowser's 5000)
    // override the per-cause defaults.
    const int speciesScore = victim.getDefeatScore(static_cast<int>(cause));
    if (speciesScore > 0) {
        points = speciesScore;
    }

    if (owner) {
        owner->queueScoreAward(popupPosition, points, grantsLife);
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
    const CollisionParticipant participantA(entityA, fixtureA->GetBody());
    const CollisionParticipant participantB(entityB, fixtureB->GetBody());

    const auto isLockedMario = [](const CollisionParticipant& participant) {
        Mario* mario = participant.mario();
        return mario && mario->isCollisionLocked();
    };

    if (isLockedMario(participantA) || isLockedMario(participantB)) {
        // A dying Mario remains in the world only to render the death jump;
        // he must not resolve any new terrain, item, or enemy contacts.
        contact->SetEnabled(false);
        return;
    }

    if (isDamageGraceEnemyContact(entityA, entityB)) {
        // SetEnabled() is reset by Box2D for the next step, so this keeps the
        // enemy intangible for the entire grace window without changing the
        // normal collision filters after the timer expires.
        contact->SetEnabled(false);
        return;
    }

    if (participantA.enemy() && participantB.enemy()) {
        contact->SetEnabled(false);
        return;
    }

    if (participantA.mario() || participantB.mario()) {
        contact->SetFriction(0.0f);
    }

    if (Enemy* enemyA = participantA.enemy(); enemyA && enemyA->isDying()) {
        contact->SetEnabled(false);
    }
    if (Enemy* enemyB = participantB.enemy(); enemyB && enemyB->isDying()) {
        contact->SetEnabled(false);
    }

    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);

    Mario* mario = nullptr;
    Entity* marioObstacle = nullptr;
    b2Body* marioBody = nullptr;
    b2Body* marioObstacleBody = nullptr;
    if (participantA.mario()) {
        mario = participantA.mario();
        marioObstacle = entityB;
        marioBody = fixtureA->GetBody();
        marioObstacleBody = fixtureB->GetBody();
    } else if (participantB.mario()) {
        mario = participantB.mario();
        marioObstacle = entityA;
        marioBody = fixtureB->GetBody();
        marioObstacleBody = fixtureA->GetBody();
    }

    // A pair of independently-created, same-height blocks has a shared
    // corner. Mario's chamfer can touch that corner diagonally while another
    // contact already supports his feet, so Box2D treats the seam as a wall.
    // Disable only that diagonal contact; the top support contact remains
    // active and real walls, ledges, ceilings, and block bumps are preserved.
    if (mario && isMarioWalkableSupportSeam(mario, marioObstacle, marioBody,
                                            marioObstacleBody, contact,
                                            worldManifold)) {
        contact->SetEnabled(false);
        return;
    }

    if (Enemy* enemyA = participantA.enemy(); enemyA &&
        isWalkableSupportSeam(enemyA, entityB,
                              fixtureA->GetBody(), fixtureB->GetBody(),
                              worldManifold.normal,
                              tileMap
                            )) {

        contact->SetEnabled(false);
        return;
    }

    if (Enemy* enemyB = participantB.enemy(); enemyB &&
        isWalkableSupportSeam(enemyB, entityA,
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
    CollisionContext context(CollisionParticipant(entityA, bodyA),
                             CollisionParticipant(entityB, bodyB),
                             contact, worldManifold.normal);

    // Compatibility callbacks observe the lifecycle but never own policy.
    // Invoke exactly once per participant with a normal oriented away from it.
    if (entityA) entityA->onCollisionBegin(entityB, contact, worldManifold.normal);
    if (entityB) entityB->onCollisionBegin(entityA, contact, -worldManifold.normal);

    dispatch(context, tileMap);
}

void CollisionManager::end(b2Contact* contact) {
    if (!contact || !contact->GetFixtureA() || !contact->GetFixtureB()) return;
    Entity* entityA = entityFromBody(contact->GetFixtureA()->GetBody());
    Entity* entityB = entityFromBody(contact->GetFixtureB()->GetBody());
    if (entityA) entityA->onCollisionEnd(entityB, contact);
    if (entityB) entityB->onCollisionEnd(entityA, contact);
}

void CollisionManager::dispatch(CollisionContext& context, TileMap& tileMap) {
    b2Contact* contact = context.contact();
    if (!contact) return;

    const CollisionParticipant& participantA = context.first();
    const CollisionParticipant& participantB = context.second();
    Entity* entityA = participantA.entity();
    Entity* entityB = participantB.entity();
    b2Body* bodyA = participantA.body();
    b2Body* bodyB = participantB.body();
    if (!bodyA || !bodyB) return;

    // BeginContact runs before PreSolve. Guard the gameplay dispatch here as
    // well so a newly entered enemy contact cannot stomp, kick, or queue a
    // second damage event during the post-hit grace window.
    if (isDamageGraceEnemyContact(entityA, entityB)) {
        return;
    }

    // Enemy-owned projectiles (Hammer Bro's hammers, Bowser's fire breath)
    // only ever interact with a player; they fly through terrain, enemies,
    // and Mario's own fireballs, and a star-powered player is immune.
    const CollisionParticipant* enemyProjectile = nullptr;
    const CollisionParticipant* projectileTarget = nullptr;
    if (participantA.has(Entity::Capability::ENEMY_PROJECTILE)) {
        enemyProjectile = &participantA;
        projectileTarget = &participantB;
    } else if (participantB.has(Entity::Capability::ENEMY_PROJECTILE)) {
        enemyProjectile = &participantB;
        projectileTarget = &participantA;
    }

    if (enemyProjectile) {
        if (Mario* victim = projectileTarget->mario()) {
            if (!victim->isCollisionLocked() && !victim->isStarInvincible()) {
                victim->queuePowerDown();
            }
        }
        return;
    }

    b2Vec2 normal = context.normalFrom(participantA);

    // PvP: a contact between the two fighters is a duel event, not a regular
    // Mario-vs-entity collision. Handled first so the second Mario is never
    // dispatched through the single-player obstacle branches.
    if (participantA.mario() && participantB.mario()) {
        handlePvpPlayerCollision(participantA.mario(),
                                 participantB.mario(),
                                 contact);
        return;
    }

    // Handle FireBall collisions if present
    FireBall* fireBall = nullptr;
    Entity* target = nullptr;
    b2Body* fireBallBody = nullptr;

    if (participantA.fireBall()) {
        fireBall = participantA.fireBall();
        target = entityB;
        fireBallBody = bodyA;
    } else if (participantB.fireBall()) {
        fireBall = participantB.fireBall();
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

        // Fireballs always pass through their shooter. In PvP arenas each
        // fighter carries his own collision group, so a fireball that reaches
        // the opponent is a queued knockback hit instead of a pass-through.
        const CollisionParticipant& targetParticipant =
            fireBallBody == bodyA ? participantB : participantA;
        if (Mario* victim = targetParticipant.mario()) {
            if (fireBall->getOwner() != victim && !victim->isCollisionLocked()) {
                PvpHit hit;
                hit.cause = PvpHit::Cause::FIREBALL;
                hit.attacker = fireBall->getOwner();
                hit.victim = victim;
                hit.position = victim->getPosition() +
                               sf::Vector2f(victim->getSize().x / 2.f, 0.f);
                queuePvpHit(hit);
                fireBall->deactivate();
            }
            return;
        }

        if (target) {
            if (Enemy* enemy = targetParticipant.enemy()) {
                // A defeated enemy may keep its body for a short death
                // animation. It is no longer a gameplay target, so the
                // projectile must pass through it and remain active.
                if (enemy->isDying()) {
                    return;
                }

                // A fireproof shell (Buzzy Beetle) blocks the shot: the
                // fireball bursts against it, the enemy survives.
                if (enemy->hasCapability(Entity::Capability::FIREPROOF)) {
                    fireBall->deactivate();
                    return;
                }

                // Multi-hit bosses (Bowser) chip their health outside the
                // one-shot defeat transaction; the killing blow pays the
                // species price.
                if (enemy->getFireballHealth() > 0) {
                    enemy->onFireHit();
                    fireBall->deactivate();
                    if (Mario* owner = fireBall->getOwner()) {
                        const sf::Vector2f popupPosition =
                            enemy->getPosition() +
                            sf::Vector2f(enemy->getSize().x / 2.f, 0.f);
                        const int points = enemy->isDying()
                            ? enemy->getDefeatScore(static_cast<int>(DefeatCause::FIREBALL))
                            : ScoreRules::pointsFor(DefeatCause::FIREBALL);
                        owner->queueScoreAward(popupPosition, points, false);
                    }
                    return;
                }

                // Only consume a FireBall when the defeat transaction really
                // claimed a living enemy. This also protects against a
                // duplicate contact from another fixture in the same frame.
                if (defeatEnemy(*enemy, DefeatCause::FIREBALL,
                                fireBall->getOwner())) {
                    fireBall->deactivate();
                }
                return;
            }
        }

        // Floor contact: Bounce FireBall
        if (normal.y > 0.5f) {
            fireBall->bounce(sf::Vector2f(normal.x, normal.y));
        }
        // Wall or Overhead Ceiling contact: Deactivate FireBall (require small lifetime grace period so initial spawn doesn't instantly die)
        else if (std::abs(normal.x) > 0.5f || normal.y < -0.5f) {
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

    if (participantA.mario()) {
        mario = participantA.mario();
        otherEntity = entityB;
        marioBody = bodyA;
    } else if (participantB.mario()) {
        mario = participantB.mario();
        otherEntity = entityA;
        marioBody = bodyB;
    }

    if (mario && marioBody) {
        if (mario->isCollisionLocked()) {
            contact->SetEnabled(false);
            return;
        }

        // Item pickup is resolved here, alongside every other Mario gameplay
        // collision. Level's overlap sweep remains a fallback for items whose
        // sensor contact was not reported by Box2D, while Item::isCollectible()
        // makes the operation idempotent.
        const CollisionParticipant& otherParticipant =
            marioBody == bodyA ? participantB : participantA;
        if (Item* item = otherParticipant.item()) {
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
    if (participantA.enemy() && participantB.enemy()) {
        Enemy* enemyA = participantA.enemy();
        Enemy* enemyB = participantB.enemy();

        if (enemyA->isDying() || enemyB->isDying()) {
            return;
        }

        auto tryShellKill = [](Enemy* attacker, Enemy* victim) -> bool {
            if (!attacker || !victim) {
                return false;
            }

            if (!attacker->hasCapability(Entity::Capability::SHELL_LIKE) ||
                victim == attacker) {
                return false;
            }

            Koopa* koopa = dynamic_cast<Koopa*>(attacker);
            if (!koopa) return false;

            if (!koopa->isShellSliding()) {
                return false;
            }

            const int streak = koopa->getShellKillStreak();
            const bool defeated = CollisionManager::defeatEnemy(*victim,
                                                                 DefeatCause::SHELL,
                                                                 koopa->getDefeatOwner(),
                                                                 streak);
            if (defeated) {
                koopa->incrementShellKillStreak();
            }
            return defeated;
        };

        tryShellKill(enemyA, enemyB);
        tryShellKill(enemyB, enemyA);

        return;
    }

    // Handle Enemy ↔ Wall / Static Body collisions (Task 3.1)
    if (participantA.enemy()) {
        handleEnemyWallCollision(participantA.enemy(), entityB,
                                 bodyA, bodyB,
                                 normal,
                                 tileMap
                                 );
    } else if (participantB.enemy()) {
        handleEnemyWallCollision(participantB.enemy(), entityA,
                                 bodyB, bodyA,
                                 -normal,
                                 tileMap
                                 );
    }

    // Handle Item ↔ Wall / Static Body collisions
    if (participantA.item()) {
        b2Vec2 itemNormal = normal;
        if (Mushroom* mushroom = participantA.mushroom()) {
            if (std::abs(itemNormal.x) > 0.1f) mushroom->onWallCollision();
        } else if (Star* star = participantA.star()) {
            if (std::abs(itemNormal.x) > 0.1f) star->onWallCollision();
            if (itemNormal.y > 0.8f) star->onGroundCollision();
        }
    } else if (participantB.item()) {
        b2Vec2 itemNormal = -normal;
        if (Mushroom* mushroom = participantB.mushroom()) {
            if (std::abs(itemNormal.x) > 0.1f) mushroom->onWallCollision();
        } else if (Star* star = participantB.star()) {
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
    b2Body* otherBody = (contact->GetFixtureA()->GetBody() == marioBody)
                        ? contact->GetFixtureB()->GetBody()
                        : contact->GetFixtureA()->GetBody();
    const CollisionParticipant otherParticipant(other, otherBody);

    if (!other) {
        // Head bump check against TileMap blocks from below
        const uintptr_t otherPtr = otherBody ? otherBody->GetUserData().pointer : 0;

        if (TileMap::isTileUserData(otherPtr)) {
            if (normal.y < BOTTOM_BLOCK_NORMAL_THRESHOLD && marioVel.y < -0.1f) {
                queueTileBlockHitFromContact(tileMap, mario, marioBody, contact, worldManifold);
            }
        }
    }

    if (Springboard* springboard = otherParticipant.springboard()) {
        if (normal.y > TOP_STOMP_NORMAL_THRESHOLD && marioVel.y >= -0.1f) {
            bool isHoldingJump = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) ||
                                 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) ||
                                 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
            springboard->triggerSpring(*mario, isHoldingJump);
            return;
        }
    }

    // Top stomp check. Grounded state itself is refreshed from all active
    // Box2D contacts after each completed physics step.
    bool isStomp = false;
    if (Enemy* enemy = otherParticipant.enemy()) {
        if (enemy->isDying()) {
            return;
        }

        // Star invincibility is a separate gameplay authority from damage
        // grace. It defeats the enemy through the same cause/score/event
        // transaction regardless of whether the contact looks like a stomp
        // or a side hit. Indestructible enemies (Podoboo) simply pass by.
        if (mario->isStarInvincible()) {
            if (!enemy->hasCapability(Entity::Capability::INDESTRUCTIBLE)) {
                CollisionManager::defeatEnemy(*enemy, DefeatCause::STAR, mario);
            }
            mario->clearGroundedState();
            return;
        }

        if (enemy->getSubtype() == Entity::EntitySubtype::PIRANHA_PLANT) {
            mario->queuePowerDown();
            return;
        }

        // Unstompable enemies (swimming Cheep Cheeps, Bloopers, Podoboo)
        // always power Mario down on contact.
        if (!enemy->hasCapability(Entity::Capability::STOMPABLE)) {
            mario->queuePowerDown();
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
        Enemy* enemy = otherParticipant.enemy();
        if (!enemy) return;

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
        if (other && other->getSubtype() == Entity::EntitySubtype::QUESTION_BLOCK) {
            queueEntityBlockHit(tileMap, mario, marioBody, *other);
        }
#ifdef DEBUG
        std::cout << "[DEBUG][CollisionManager] Mario hit overhead block from below!" << std::endl;
#endif
    }

    // Lateral collision (wall contact)
    else {
        if (other) {
            if (otherParticipant.enemy()) {
                
                // Koopa Kick Logic
                if (Koopa* koopa = otherParticipant.shell()) {
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
    if (!otherParticipant.enemy() &&
        normal.y > TOP_STOMP_NORMAL_THRESHOLD &&
        std::abs(normal.x) < MAX_WALL_NORMAL_X &&
        marioVel.y >= -0.1f) {
        mario->setGrounded(true);
    }
}

void CollisionManager::handlePvpPlayerCollision(Mario* playerA,
                                                Mario* playerB,
                                                b2Contact* contact) {
    if (!playerA || !playerB) {
        return;
    }

    // A dying/locked fighter cannot score or be scored on; the pre-solve pass
    // already disables the physical contact for the loser's death jump.
    if (playerA->isCollisionLocked() || playerB->isCollisionLocked()) {
        return;
    }

    b2Body* bodyA = playerA->getBody();
    b2Body* bodyB = playerB->getBody();
    if (!bodyA || !bodyB) {
        return;
    }

    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);

    // Re-orient the manifold normal so it always points away from playerA,
    // matching the convention handleMarioCollision uses for its stomp test.
    b2Vec2 normal = worldManifold.normal;
    if (contact->GetFixtureA()->GetBody() == bodyB) {
        normal = -normal;
    }

    const auto bottomMeters = [](b2Body* body, const sf::Vector2f& size) {
        return body->GetPosition().y +
               PhysicsEngine::pixelsToMeters(size.y / 2.0f);
    };
    const auto headWindow = [](float attackerBottom,
                               b2Body* victimBody,
                               const sf::Vector2f& victimSize) {
        const float victimMid = victimBody->GetPosition().y;
        const float tolerance =
            PhysicsEngine::pixelsToMeters(victimSize.y * 0.2f);
        return attackerBottom <= victimMid + tolerance;
    };

    // Geometry: X can stomp Y when X's feet are at or above Y's midpoint —
    // the same glancing-contact fallback the enemy stomp uses.
    const bool geometryA =
        (normal.y > TOP_STOMP_NORMAL_THRESHOLD &&
         std::abs(normal.x) < MAX_WALL_NORMAL_X) ||
        headWindow(bottomMeters(bodyA, playerA->getSize()), bodyB,
                   playerB->getSize());
    const bool geometryB =
        (normal.y < -TOP_STOMP_NORMAL_THRESHOLD &&
         std::abs(normal.x) < MAX_WALL_NORMAL_X) ||
        headWindow(bottomMeters(bodyB, playerB->getSize()), bodyA,
                   playerA->getSize());

    // A scoring stomp must be a descent; rising grazes never count.
    const bool fallingA = bodyA->GetLinearVelocity().y > 0.f;
    const bool fallingB = bodyB->GetLinearVelocity().y > 0.f;

    Mario* attacker = nullptr;
    Mario* victim = nullptr;
    if (geometryA && fallingA && geometryB && fallingB) {
        // Symmetric mid-air clash: the higher feet wins outright; a perfect
        // tie scores nobody.
        const float bottomA = bottomMeters(bodyA, playerA->getSize());
        const float bottomB = bottomMeters(bodyB, playerB->getSize());
        if (bottomA < bottomB - 0.001f) {
            attacker = playerA;
            victim = playerB;
        } else if (bottomB < bottomA - 0.001f) {
            attacker = playerB;
            victim = playerA;
        }
    } else if (geometryA && fallingA) {
        attacker = playerA;
        victim = playerB;
    } else if (geometryB && fallingB) {
        attacker = playerB;
        victim = playerA;
    }

    if (!attacker || !victim) {
        return;
    }

    PvpHit hit;
    hit.cause = PvpHit::Cause::STOMP;
    hit.attacker = attacker;
    hit.victim = victim;
    hit.position = victim->getPosition() +
                   sf::Vector2f(victim->getSize().x / 2.f, 0.f);
    queuePvpHit(hit);
}
