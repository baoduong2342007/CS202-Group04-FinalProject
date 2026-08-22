/**
 * @file PvpStompTests.cpp
 * @author TV5 (Truyen)
 * @brief Player-vs-player contact tests on a real Box2D world
 * @note Covers the head-stomp duel detection, the rising-graze rejection,
 *       side-by-side neutrality and fireball ownership filtering.
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

#include <box2d/box2d.h>

#include "entities/FireBall.h"
#include "entities/Mario.h"
#include "level/TileMap.h"
#include "physics/CollisionManager.h"
#include "physics/ContactListener.h"

namespace {

constexpr float PVP_GROUP_P1 = -1.f;
constexpr float PVP_GROUP_P2 = -2.f;

/// Duel fixture: one world, one listener, two fighters with distinct groups.
struct DuelWorld {
    b2World world{b2Vec2(0.f, 25.f)};
    TileMap tileMap;
    ContactListener listener;
    Mario fighterA;
    Mario fighterB;

    DuelWorld() : listener(tileMap) {
        world.SetContactListener(&listener);

        fighterA.initPhysics(&world, b2_dynamicBody, {32.f, 32.f});
        fighterB.initPhysics(&world, b2_dynamicBody, {32.f, 32.f});
        fighterA.setFixtureCollisionGroup(
            static_cast<int16_t>(PVP_GROUP_P1));
        fighterB.setFixtureCollisionGroup(
            static_cast<int16_t>(PVP_GROUP_P2));
    }
};

void stepOnce(b2World& world) {
    world.Step(1.f / 60.f, 8, 3);
}

std::vector<PvpHit> drained() {
    return CollisionManager::consumePendingPvpHits();
}

bool testFallingHeadStompQueuesKO() {
    std::cout << "[RUNNING] testFallingHeadStompQueuesKO..." << std::endl;
    CollisionManager::clearPendingPvpHits();

    DuelWorld duel;

    // Fighter A hovers above fighter B's head, descending. Fixture half
    // height is 0.5 m; the feet sit inside B's head window.
    duel.fighterA.getBody()->SetTransform(b2Vec2(0.f, -0.42f), 0.f);
    duel.fighterB.getBody()->SetTransform(b2Vec2(0.f, 0.f), 0.f);
    duel.fighterA.getBody()->SetLinearVelocity(b2Vec2(0.f, 5.f));
    duel.fighterB.getBody()->SetLinearVelocity(b2Vec2(0.f, 0.f));

    stepOnce(duel.world);

    const std::vector<PvpHit> hits = drained();
    assert(hits.size() == 1);
    assert(hits[0].cause == PvpHit::Cause::STOMP);
    assert(hits[0].attacker == &duel.fighterA);
    assert(hits[0].victim == &duel.fighterB);

    std::cout << "[PASSED] testFallingHeadStompQueuesKO" << std::endl;
    return true;
}

bool testRisingGrazeDoesNotScore() {
    std::cout << "[RUNNING] testRisingGrazeDoesNotScore..." << std::endl;
    CollisionManager::clearPendingPvpHits();

    DuelWorld duel;

    // Same geometry, but A is rising (jumping up into B from below): the
    // descent requirement must reject the stomp.
    duel.fighterA.getBody()->SetTransform(b2Vec2(0.f, -0.42f), 0.f);
    duel.fighterB.getBody()->SetTransform(b2Vec2(0.f, 0.f), 0.f);
    duel.fighterA.getBody()->SetLinearVelocity(b2Vec2(0.f, -5.f));
    duel.fighterB.getBody()->SetLinearVelocity(b2Vec2(0.f, 0.f));

    stepOnce(duel.world);

    assert(drained().empty());

    std::cout << "[PASSED] testRisingGrazeDoesNotScore" << std::endl;
    return true;
}

bool testSideBySideContactIsNeutral() {
    std::cout << "[RUNNING] testSideBySideContactIsNeutral..." << std::endl;
    CollisionManager::clearPendingPvpHits();

    DuelWorld duel;

    // Shoulder-to-shoulder on the same height: neither fighter's feet reach
    // the other's head window and the contact normal is horizontal.
    duel.fighterA.getBody()->SetTransform(b2Vec2(-0.4f, 0.f), 0.f);
    duel.fighterB.getBody()->SetTransform(b2Vec2(0.4f, 0.f), 0.f);
    duel.fighterA.getBody()->SetLinearVelocity(b2Vec2(0.f, 2.f));
    duel.fighterB.getBody()->SetLinearVelocity(b2Vec2(0.f, 2.f));

    stepOnce(duel.world);

    assert(drained().empty());

    std::cout << "[PASSED] testSideBySideContactIsNeutral" << std::endl;
    return true;
}

bool testStompOnDyingFighterIgnored() {
    std::cout << "[RUNNING] testStompOnDyingFighterIgnored..." << std::endl;
    CollisionManager::clearPendingPvpHits();

    DuelWorld duel;

    // A dying fighter is collision-locked and must not participate in duel
    // contacts at all.
    duel.fighterB.loseLife();
    assert(duel.fighterB.isDying());

    duel.fighterA.getBody()->SetTransform(b2Vec2(0.f, -0.42f), 0.f);
    duel.fighterB.getBody()->SetTransform(b2Vec2(0.f, 0.f), 0.f);
    duel.fighterA.getBody()->SetLinearVelocity(b2Vec2(0.f, 5.f));
    duel.fighterB.getBody()->SetLinearVelocity(b2Vec2(0.f, 0.f));

    stepOnce(duel.world);

    assert(drained().empty());

    std::cout << "[PASSED] testStompOnDyingFighterIgnored" << std::endl;
    return true;
}

bool testFireballPassesOwnerHitsOpponent() {
    std::cout << "[RUNNING] testFireballPassesOwnerHitsOpponent..." << std::endl;

    // The shooter stays out of reach; only his fireball meets a fighter.
    {
        CollisionManager::clearPendingPvpHits();
        DuelWorld duel;

        duel.fighterA.getBody()->SetTransform(b2Vec2(-2.f, 0.f), 0.f);
        duel.fighterB.getBody()->SetTransform(b2Vec2(2.f, 0.f), 0.f);

        // Fireball parked on the shooter: shared negative group -> the pair
        // never even generates a contact.
        auto ownFireball = std::make_unique<FireBall>(
            sf::Vector2f(-68.f, -8.f), Direction::RIGHT, &duel.world);
        ownFireball->setOwner(&duel.fighterA);
        ownFireball->setCollisionGroup(
            duel.fighterA.getFixtureCollisionGroup());
        ownFireball->setVelocity({0.f, 0.f});

        stepOnce(duel.world);

        assert(drained().empty());
        assert(ownFireball->isActive());
    }

    // The same fireball on the opponent: contact is generated, the hit is
    // queued and the projectile is consumed.
    {
        CollisionManager::clearPendingPvpHits();
        DuelWorld duel;

        duel.fighterA.getBody()->SetTransform(b2Vec2(-2.f, 0.f), 0.f);
        duel.fighterB.getBody()->SetTransform(b2Vec2(2.f, 0.f), 0.f);

        auto shot = std::make_unique<FireBall>(
            sf::Vector2f(52.f, -8.f), Direction::RIGHT, &duel.world);
        shot->setOwner(&duel.fighterA);
        shot->setCollisionGroup(duel.fighterA.getFixtureCollisionGroup());
        shot->setVelocity({0.f, 0.f});

        stepOnce(duel.world);

        const std::vector<PvpHit> hits = drained();
        assert(hits.size() == 1);
        assert(hits[0].cause == PvpHit::Cause::FIREBALL);
        assert(hits[0].attacker == &duel.fighterA);
        assert(hits[0].victim == &duel.fighterB);
        assert(!shot->isActive());
    }

    std::cout << "[PASSED] testFireballPassesOwnerHitsOpponent" << std::endl;
    return true;
}

bool testFireballsPassThroughEachOther() {
    std::cout << "[RUNNING] testFireballsPassThroughEachOther..." << std::endl;
    CollisionManager::clearPendingPvpHits();

    DuelWorld duel;

    auto p1Shot = std::make_unique<FireBall>(
        sf::Vector2f(-8.f, -8.f), Direction::RIGHT, &duel.world);
    p1Shot->setOwner(&duel.fighterA);
    p1Shot->setCollisionGroup(duel.fighterA.getFixtureCollisionGroup());
    p1Shot->setVelocity({0.f, 0.f});

    auto p2Shot = std::make_unique<FireBall>(
        sf::Vector2f(-8.f, -8.f), Direction::LEFT, &duel.world);
    p2Shot->setOwner(&duel.fighterB);
    p2Shot->setCollisionGroup(duel.fighterB.getFixtureCollisionGroup());
    p2Shot->setVelocity({0.f, 0.f});

    stepOnce(duel.world);

    // The projectile category mask excludes the projectile category itself,
    // so opposing shots cross paths instead of exploding mid-air.
    assert(p1Shot->isActive());
    assert(p2Shot->isActive());
    assert(drained().empty());

    std::cout << "[PASSED] testFireballsPassThroughEachOther" << std::endl;
    return true;
}

} // namespace

int main() {
    const bool success = testFallingHeadStompQueuesKO() &&
                         testRisingGrazeDoesNotScore() &&
                         testSideBySideContactIsNeutral() &&
                         testStompOnDyingFighterIgnored() &&
                         testFireballPassesOwnerHitsOpponent() &&
                         testFireballsPassThroughEachOther();

    if (success) {
        std::cout << "All PvpStomp tests passed successfully!" << std::endl;
        return 0;
    }

    std::cout << "PvpStomp tests FAILED." << std::endl;
    return 1;
}
