/**
 * @file FireBallRequestTests.cpp
 * @author TV3, TV5
 * @brief Single-API FireBall limit, cooldown, world-lock, owner and SFX tests.
 */

#include <cassert>

#include <box2d/box2d.h>

#include "core/SoundManager.h"
#include "entities/FireBall.h"
#include "level/Level.h"
#include "patterns/EventBus.h"
#include "patterns/IObserver.h"

namespace {
class ShotCounter final : public IObserver {
public:
    ShotCounter() {
        EventBus::getInstance().subscribe(EventType::FIREBALL_SHOT, this);
    }
    ~ShotCounter() override {
        EventBus::getInstance().unsubscribe(EventType::FIREBALL_SHOT, this);
    }
    void onNotify(EventType event) override {
        if (event == EventType::FIREBALL_SHOT) ++count;
    }
    int count = 0;
};

class LockedShotListener final : public b2ContactListener {
public:
    LockedShotListener(Level& level, Mario& mario)
        : m_level(level), m_mario(mario) {}
    void BeginContact(b2Contact*) override {
        if (!attempted) {
            attempted = true;
            accepted = m_level.requestFireBallShot(m_mario);
        }
    }
    bool attempted = false;
    bool accepted = false;
private:
    Level& m_level;
    Mario& m_mario;
};

void testLimitReleaseAndSingleSfx() {
    SoundManager& sound = SoundManager::getInstance();
    sound.resetDiagnosticCounters();
    ShotCounter events;
    Level level;
    assert(level.loadFromFile("levels/level1.txt"));
    Mario& mario = *level.getMario();
    mario.setMarioState(MarioState::FIRE_SMALL);

    assert(level.requestFireBallShot(mario));
    assert(level.getActiveFireBallCount() == 1);
    assert(events.count == 1);
    assert(sound.getSoundPlayRequestCount("fireball") == 1);
    assert(!level.requestFireBallShot(mario)); // cooldown rejection
    assert(events.count == 1);

    mario.update(0.3f);
    assert(level.requestFireBallShot(mario));
    mario.update(0.3f);
    assert(level.getActiveFireBallCount() == 2);
    assert(!level.requestFireBallShot(mario)); // hard limit rejection
    assert(events.count == 2);
    assert(sound.getSoundPlayRequestCount("fireball") == 2);

    for (auto& entity : level.getEntities()) {
        if (entity && entity->isFireBall() && entity->isActive()) {
            static_cast<FireBall*>(entity.get())->deactivate(false);
            break;
        }
    }
    assert(level.getActiveFireBallCount() == 1);
    assert(level.requestFireBallShot(mario));
    assert(level.getActiveFireBallCount() == 2);
    assert(events.count == 3);
    assert(sound.getSoundPlayRequestCount("fireball") == 3);
}

void testWorldLockedRequestReservesCooldownAndOwner() {
    SoundManager& sound = SoundManager::getInstance();
    sound.resetDiagnosticCounters();
    ShotCounter events;
    Level level;
    assert(level.loadFromFile("levels/level1.txt"));
    Mario& mario = *level.getMario();
    mario.setMarioState(MarioState::FIRE_SMALL);
    mario.moveLeft();

    b2World* world = mario.getBody()->GetWorld();
    LockedShotListener listener(level, mario);
    world->SetContactListener(&listener);

    b2BodyDef bodyDefinition;
    bodyDefinition.type = b2_staticBody;
    bodyDefinition.position = mario.getBody()->GetPosition();
    b2Body* overlap = world->CreateBody(&bodyDefinition);
    b2PolygonShape overlapShape;
    overlapShape.SetAsBox(0.25f, 0.25f);
    b2FixtureDef fixtureDefinition;
    fixtureDefinition.shape = &overlapShape;
    fixtureDefinition.isSensor = true;
    overlap->CreateFixture(&fixtureDefinition);

    world->Step(1.f / 60.f, 8, 3);
    assert(listener.attempted && listener.accepted);
    assert(level.getPendingFireBallCount() == 1);
    assert(level.getActiveFireBallCount() == 0);
    assert(mario.getFireCooldownRemaining() > 0.f);
    assert(events.count == 0); // creation, not request acceptance, emits the edge
    assert(sound.getSoundPlayRequestCount("fireball") == 0);

    world->SetContactListener(nullptr);
    level.update(0.f);
    assert(level.getPendingFireBallCount() == 0);
    assert(level.getActiveFireBallCount() == 1);
    assert(events.count == 1);
    assert(sound.getSoundPlayRequestCount("fireball") == 1);

    FireBall* spawned = nullptr;
    for (auto& entity : level.getEntities()) {
        if (entity && entity->isFireBall() && entity->isActive()) {
            spawned = static_cast<FireBall*>(entity.get());
            break;
        }
    }
    assert(spawned);
    assert(spawned->getOwner() == &mario);
    assert(spawned->getDirection() == Direction::LEFT);
}
} // namespace

int main() {
    testLimitReleaseAndSingleSfx();
    testWorldLockedRequestReservesCooldownAndOwner();
    return 0;
}
