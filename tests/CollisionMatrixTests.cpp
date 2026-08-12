/**
 * @file CollisionMatrixTests.cpp
 * @author TV3, TV5
 * @brief Runtime collision matrix with fixture creation order A/B and idempotence.
 */

#include <cassert>
#include <cmath>
#include <memory>

#include <box2d/box2d.h>

#include "core/ScoreRules.h"
#include "core/SoundManager.h"
#include "core/TextureManager.h"
#include "entities/FireBall.h"
#include "entities/Goomba.h"
#include "entities/Koopa.h"
#include "entities/Mario.h"
#include "entities/PiranhaPlant.h"
#include "level/TileMap.h"
#include "patterns/EventBus.h"
#include "patterns/IObserver.h"
#include "physics/ContactListener.h"

namespace {
class CollisionEvents final : public IObserver {
public:
    CollisionEvents() {
        auto& bus = EventBus::getInstance();
        for (EventType event : {EventType::ENEMY_STOMPED,
                                EventType::SHELL_KICKED,
                                EventType::ENEMY_DEFEATED_BY_SHELL,
                                EventType::ENEMY_DEFEATED_BY_FIREBALL,
                                EventType::ENEMY_DEFEATED_BY_STAR,
                                EventType::ENEMY_DEFEATED_BY_BLOCK,
                                EventType::PLAYER_DIED}) {
            bus.subscribe(event, this);
        }
    }
    ~CollisionEvents() override {
        auto& bus = EventBus::getInstance();
        for (EventType event : {EventType::ENEMY_STOMPED,
                                EventType::SHELL_KICKED,
                                EventType::ENEMY_DEFEATED_BY_SHELL,
                                EventType::ENEMY_DEFEATED_BY_FIREBALL,
                                EventType::ENEMY_DEFEATED_BY_STAR,
                                EventType::ENEMY_DEFEATED_BY_BLOCK,
                                EventType::PLAYER_DIED}) {
            bus.unsubscribe(event, this);
        }
    }
    void onNotify(EventType event) override {
        if (event == EventType::ENEMY_STOMPED) ++stomp;
        else if (event == EventType::SHELL_KICKED) ++shellKick;
        else if (event == EventType::ENEMY_DEFEATED_BY_SHELL) ++shellKill;
        else if (event == EventType::ENEMY_DEFEATED_BY_FIREBALL) ++fireKill;
        else if (event == EventType::ENEMY_DEFEATED_BY_STAR) ++starKill;
        else if (event == EventType::ENEMY_DEFEATED_BY_BLOCK) ++blockKill;
        else if (event == EventType::PLAYER_DIED) ++death;
    }
    int stomp = 0;
    int shellKick = 0;
    int shellKill = 0;
    int fireKill = 0;
    int starKill = 0;
    int blockKill = 0;
    int death = 0;
};

void stepTwice(b2World& world) {
    world.Step(1.f / 60.f, 8, 3);
    world.Step(1.f / 60.f, 8, 3);
}

// Exposes the sprite scale so the FireBall-defeat retraction can be checked to
// stay upright (positive Y scale) and mirrored only horizontally (never the
// vertical flip that used to render the plant upside down).
class PiranhaSpriteProbe final : public PiranhaPlant {
public:
    using PiranhaPlant::PiranhaPlant;
    float spriteScaleY() const {
        return m_sprite ? m_sprite->getScale().y : 0.0f;
    }
    float spriteScaleX() const {
        return m_sprite ? m_sprite->getScale().x : 0.0f;
    }
};

void runPiranhaFireRetractOnlyUpright() {
    SoundManager::getInstance().resetDiagnosticCounters();
    b2World world({0.f, 0.f});
    TileMap tileMap;
    ContactListener listener(tileMap);
    world.SetContactListener(&listener);
    Mario owner;
    PiranhaSpriteProbe plant(sf::Vector2f(0.f, 0.f), &world);
    plant.setTextureManager(TextureManager::getInstance());
    CollisionEvents events;

    auto fireBall = std::make_unique<FireBall>(
        sf::Vector2f(0.f, 0.f), Direction::RIGHT, &world);
    fireBall->setOwner(&owner);
    fireBall->setVelocity({0.f, 0.f});
    stepTwice(world);

    assert(plant.isDying());
    assert(plant.getBody()->IsEnabled()); // still enabled before the safe point
    assert(events.fireKill == 1);

    // Safe-point update disables the body and starts the retraction with an
    // upright (positive Y) sprite — never upside down and never re-emerging.
    const float baseY = plant.getPosition().y;
    plant.update(0.25f);
    assert(!plant.getBody()->IsEnabled());
    assert(plant.spriteScaleY() > 0.0f);
    assert(plant.spriteScaleX() != 0.0f);

    // The plant must only move DOWN toward the pipe base. A decreasing Y would
    // mean it started rising again (re-emergence), which is forbidden after a
    // FireBall defeat.
    float prevY = plant.getPosition().y;
    bool everRose = false;
    for (int i = 0; i < 8 && !plant.shouldRemove(); ++i) {
        plant.update(0.5f);
        const float currentY = plant.getPosition().y;
        if (currentY < prevY - 0.01f) {
            everRose = true;
        }
        prevY = currentY;
    }
    assert(!everRose);
    assert(plant.getPosition().y >= baseY - 0.01f);
    plant.update(1.f);
    assert(plant.shouldRemove());
    assert(owner.getScore() == ScoreRules::pointsFor(DefeatCause::FIREBALL));
}

void runStomp(bool marioFixtureFirst) {
    SoundManager::getInstance().resetDiagnosticCounters();
    b2World world({0.f, 0.f});
    TileMap tileMap;
    ContactListener listener(tileMap);
    world.SetContactListener(&listener);
    std::unique_ptr<Mario> mario;
    std::unique_ptr<Goomba> goomba;
    if (marioFixtureFirst) {
        mario = std::make_unique<Mario>(sf::Vector2f(0.f, 0.f),
                                        sf::Vector2f(28.f, 30.f));
        mario->initPhysics(&world, b2_dynamicBody, {28.f, 30.f});
        goomba = std::make_unique<Goomba>(sf::Vector2f(0.f, 25.f), &world);
    } else {
        goomba = std::make_unique<Goomba>(sf::Vector2f(0.f, 25.f), &world);
        mario = std::make_unique<Mario>(sf::Vector2f(0.f, 0.f),
                                        sf::Vector2f(28.f, 30.f));
        mario->initPhysics(&world, b2_dynamicBody, {28.f, 30.f});
    }
    CollisionEvents events;
    stepTwice(world);
    assert(goomba->isDead());
    assert(mario->getScore() == ScoreRules::pointsFor(DefeatCause::STOMP));
    assert(events.stomp == 1);
    assert(SoundManager::getInstance().getSoundPlayRequestCount("stomp") == 1);
}

void runSideHit(bool marioFixtureFirst) {
    SoundManager::getInstance().resetDiagnosticCounters();
    b2World world({0.f, 0.f});
    TileMap tileMap;
    ContactListener listener(tileMap);
    world.SetContactListener(&listener);
    std::unique_ptr<Mario> mario;
    std::unique_ptr<Goomba> goomba;
    if (marioFixtureFirst) {
        mario = std::make_unique<Mario>(sf::Vector2f(0.f, 0.f),
                                        sf::Vector2f(28.f, 30.f));
        mario->initPhysics(&world, b2_dynamicBody, {28.f, 30.f});
        goomba = std::make_unique<Goomba>(sf::Vector2f(24.f, 0.f), &world);
    } else {
        goomba = std::make_unique<Goomba>(sf::Vector2f(24.f, 0.f), &world);
        mario = std::make_unique<Mario>(sf::Vector2f(0.f, 0.f),
                                        sf::Vector2f(28.f, 30.f));
        mario->initPhysics(&world, b2_dynamicBody, {28.f, 30.f});
    }
    CollisionEvents events;
    const int lives = mario->getLives();
    world.Step(1.f / 60.f, 8, 3);
    mario->update(0.f); // flush the world-lock-safe queued damage transaction
    world.Step(1.f / 60.f, 8, 3);
    assert(mario->getLives() == lives - 1);
    assert(mario->getScore() == 0);
    assert(events.stomp == 0);
    assert(events.death == 1);
    assert(SoundManager::getInstance().getSoundPlayRequestCount("death") == 1);
}

void runDamageGracePassThrough(bool marioFixtureFirst) {
    b2World world({0.f, 0.f});
    TileMap tileMap;
    ContactListener listener(tileMap);
    world.SetContactListener(&listener);

    std::unique_ptr<Mario> mario;
    std::unique_ptr<Goomba> goomba;
    if (marioFixtureFirst) {
        mario = std::make_unique<Mario>(sf::Vector2f(0.f, 0.f),
                                        sf::Vector2f(28.f, 30.f));
        mario->setMarioState(MarioState::SUPER);
        mario->initPhysics(&world, b2_dynamicBody, {28.f, 60.f});
        goomba = std::make_unique<Goomba>(sf::Vector2f(24.f, 0.f), &world);
    } else {
        goomba = std::make_unique<Goomba>(sf::Vector2f(24.f, 0.f), &world);
        mario = std::make_unique<Mario>(sf::Vector2f(0.f, 0.f),
                                        sf::Vector2f(28.f, 30.f));
        mario->setMarioState(MarioState::SUPER);
        mario->initPhysics(&world, b2_dynamicBody, {28.f, 60.f});
    }

    const int lives = mario->getLives();

    // First contact downgrades Super Mario and starts the post-hit grace
    // window. The next contact must be non-solid, not merely damage-free.
    world.Step(1.f / 60.f, 8, 3);
    mario->update(0.f);
    assert(mario->getMarioState() == MarioState::SMALL);
    assert(mario->isDamageImmune());

    // Re-enter from the side while grace is active. The enemy moving into
    // Mario must not change Mario's position through the contact solver.
    mario->setPosition({0.f, 0.f});
    mario->setVelocity({0.f, 0.f});
    goomba->setPosition({24.f, 0.f});
    goomba->setVelocity({-120.f, 0.f});
    const b2Vec2 beforePassThrough = mario->getBody()->GetPosition();
    world.Step(1.f / 60.f, 8, 3);
    const b2Vec2 afterPassThrough = mario->getBody()->GetPosition();

    assert(std::abs(afterPassThrough.x - beforePassThrough.x) < 0.0001f);
    assert(std::abs(afterPassThrough.y - beforePassThrough.y) < 0.0001f);
    assert(mario->getLives() == lives);

    // A newly entered top contact is also ignored during grace; otherwise
    // BeginContact could still stomp the enemy before PreSolve disables it.
    mario->setPosition({0.f, 0.f});
    goomba->setPosition({0.f, 25.f});
    goomba->setVelocity({0.f, 0.f});
    world.Step(1.f / 60.f, 8, 3);
    assert(!goomba->isDead());
}

void runShell(bool shellFixtureFirst) {
    SoundManager::getInstance().resetDiagnosticCounters();
    b2World world({0.f, 0.f});
    TileMap tileMap;
    ContactListener listener(tileMap);
    world.SetContactListener(&listener);
    Mario owner;
    std::unique_ptr<Koopa> shell;
    std::unique_ptr<Goomba> victim;
    if (shellFixtureFirst) {
        shell = std::make_unique<Koopa>(sf::Vector2f(0.f, 0.f), &world);
        victim = std::make_unique<Goomba>(sf::Vector2f(0.f, 0.f), &world);
    } else {
        victim = std::make_unique<Goomba>(sf::Vector2f(0.f, 0.f), &world);
        shell = std::make_unique<Koopa>(sf::Vector2f(0.f, 0.f), &world);
    }
    CollisionEvents events;
    shell->onStomp();
    shell->update(0.f);
    shell->setDefeatOwner(&owner);
    shell->kick(Direction::RIGHT);
    shell->setVelocity({0.f, 0.f});
    stepTwice(world);
    assert(victim->isDead());
    assert(victim->isDying());
    assert(!victim->shouldRemove());
    assert(victim->getBody()->GetLinearVelocity().y < 0.f);
    assert(owner.getScore() == ScoreRules::pointsFor(DefeatCause::SHELL));
    assert(events.shellKick == 1);
    assert(events.shellKill == 1);
    assert(SoundManager::getInstance().getSoundPlayRequestCount("shell_kick") == 1);
    assert(SoundManager::getInstance().getSoundPlayRequestCount("shell_kill") == 1);
}

void runIdleShellStaysStationary() {
    b2World world({0.f, 0.f});
    Koopa shell({0.f, 0.f}, &world);

    shell.onStomp();
    // Simulate a horizontal impulse left by the stomp contact solver. The
    // idle-shell update must clear it before the shell can start sliding.
    shell.setVelocity({120.f, 0.f});
    shell.update(1.f / 60.f);

    assert(shell.getState() == KoopaState::SHELL_IDLE);
    assert(std::abs(shell.getVelocity().x) < 0.001f);

    shell.kick(Direction::RIGHT);
    assert(shell.isShellSliding());
    assert(shell.getVelocity().x > 0.f);
}

void runKoopaPitFallsPastPitWall() {
    b2World world({0.f, 25.f});
    TileMap tileMap;
    assert(tileMap.loadFromFile("levels/level1.txt"));
    tileMap.createPhysicsBodies(&world);
    ContactListener listener(tileMap);
    world.SetContactListener(&listener);

    constexpr float gapStart = 69.f * 32.f;
    constexpr float floorTop = 13.f * 32.f;

    Koopa koopa({0.f, 0.f}, &world, LevelTheme::UNDERGROUND);
    koopa.setTileMap(&tileMap);
    koopa.setFacingDirection(Direction::RIGHT);
    koopa.setPosition({gapStart + 28.f, floorTop - 48.f});

    bool reversedAtWall = false;
    for (int frame = 0; frame < 60; ++frame) {
        world.Step(1.f / 60.f, 8, 3);
        koopa.update(1.f / 60.f);
        reversedAtWall = reversedAtWall ||
                         koopa.getFacingDirection() == Direction::LEFT;
    }

    assert(reversedAtWall);
    assert(koopa.getPosition().y > floorTop);
    assert(koopa.getVelocity().y > 0.f);
}

void runFireBall(bool projectileFixtureFirst) {
    SoundManager::getInstance().resetDiagnosticCounters();
    b2World world({0.f, 0.f});
    TileMap tileMap;
    ContactListener listener(tileMap);
    world.SetContactListener(&listener);
    Mario owner;
    std::unique_ptr<FireBall> fireBall;
    std::unique_ptr<Goomba> victim;
    if (projectileFixtureFirst) {
        fireBall = std::make_unique<FireBall>(sf::Vector2f(0.f, 0.f),
                                              Direction::RIGHT, &world);
        victim = std::make_unique<Goomba>(sf::Vector2f(0.f, 0.f), &world);
    } else {
        victim = std::make_unique<Goomba>(sf::Vector2f(0.f, 0.f), &world);
        fireBall = std::make_unique<FireBall>(sf::Vector2f(0.f, 0.f),
                                              Direction::RIGHT, &world);
    }
    CollisionEvents events;
    fireBall->setOwner(&owner);
    fireBall->setVelocity({0.f, 0.f});
    stepTwice(world);
    assert(victim->isDead());
    assert(owner.getScore() == ScoreRules::pointsFor(DefeatCause::FIREBALL));
    assert(events.fireKill == 1);
    assert(SoundManager::getInstance().getSoundPlayRequestCount("enemy_fireball") == 1);
}

void runFireBallPassesDeadEnemy(bool projectileFixtureFirst) {
    SoundManager::getInstance().resetDiagnosticCounters();
    b2World world({0.f, 0.f});
    TileMap tileMap;
    ContactListener listener(tileMap);
    world.SetContactListener(&listener);
    Mario owner;
    std::unique_ptr<FireBall> firstFireBall;
    std::unique_ptr<Goomba> victim;

    if (projectileFixtureFirst) {
        firstFireBall = std::make_unique<FireBall>(
            sf::Vector2f(0.f, 0.f), Direction::RIGHT, &world);
        victim = std::make_unique<Goomba>(sf::Vector2f(0.f, 0.f), &world);
    } else {
        victim = std::make_unique<Goomba>(sf::Vector2f(0.f, 0.f), &world);
        firstFireBall = std::make_unique<FireBall>(
            sf::Vector2f(0.f, 0.f), Direction::RIGHT, &world);
    }

    CollisionEvents events;
    firstFireBall->setOwner(&owner);
    firstFireBall->setVelocity({0.f, 0.f});
    stepTwice(world);
    assert(victim->isDying());
    assert(!firstFireBall->isActive());
    assert(events.fireKill == 1);

    // The dead enemy body remains temporarily for its flipped animation. A
    // later projectile must pass through its sensor and stay active.
    auto secondFireBall = std::make_unique<FireBall>(
        sf::Vector2f(0.f, 0.f), Direction::RIGHT, &world);
    secondFireBall->setOwner(&owner);
    secondFireBall->setVelocity({0.f, 0.f});
    world.Step(1.f / 60.f, 8, 3);

    assert(secondFireBall->isActive());
    assert(!secondFireBall->isPendingDestroy());
    assert(owner.getScore() == ScoreRules::pointsFor(DefeatCause::FIREBALL));
    assert(events.fireKill == 1);
    assert(SoundManager::getInstance().getSoundPlayRequestCount("enemy_fireball") == 1);
}

void runPiranhaFireBall(bool projectileFixtureFirst) {
    SoundManager::getInstance().resetDiagnosticCounters();
    b2World world({0.f, 0.f});
    TileMap tileMap;
    ContactListener listener(tileMap);
    world.SetContactListener(&listener);
    Mario owner;
    std::unique_ptr<FireBall> fireBall;
    std::unique_ptr<PiranhaPlant> plant;

    if (projectileFixtureFirst) {
        fireBall = std::make_unique<FireBall>(
            sf::Vector2f(0.f, 0.f), Direction::RIGHT, &world);
        plant = std::make_unique<PiranhaPlant>(
            sf::Vector2f(0.f, 0.f), &world);
    } else {
        plant = std::make_unique<PiranhaPlant>(
            sf::Vector2f(0.f, 0.f), &world);
        fireBall = std::make_unique<FireBall>(
            sf::Vector2f(0.f, 0.f), Direction::RIGHT, &world);
    }

    CollisionEvents events;
    fireBall->setOwner(&owner);
    fireBall->setVelocity({0.f, 0.f});
    stepTwice(world);

    assert(plant->isDying());
    assert(plant->getBody()->IsEnabled());
    const float initialY = plant->getPosition().y;

    // This update is the first world-unlocked safe point after the contact.
    // It must disable the body and start the downward death motion without
    // calling b2Body::SetEnabled() from BeginContact.
    plant->update(0.25f);
    assert(!plant->getBody()->IsEnabled());
    assert(plant->getPosition().y >= initialY);
    assert(events.fireKill == 1);
    assert(owner.getScore() == ScoreRules::pointsFor(DefeatCause::FIREBALL));

    plant->update(1.f);
    assert(plant->shouldRemove());
}

void runBlockBumpDefeatsEnemy() {
    SoundManager::getInstance().resetDiagnosticCounters();
    b2World world({0.f, 0.f});
    TileMap tileMap;
    assert(tileMap.loadFromFile("levels/level1.txt"));
    tileMap.createPhysicsBodies(&world);

    const auto brickTiles = tileMap.findTiles('B');
    assert(!brickTiles.empty());
    const sf::Vector2i brick = brickTiles.front();
    const sf::Vector2f brickPosition = TileMap::gridToWorldPosition(brick);

    Mario mario;
    auto victim = std::make_unique<Goomba>(
        sf::Vector2f(brickPosition.x, brickPosition.y - 32.f), &world);
    Goomba* victimPtr = victim.get();
    std::vector<std::unique_ptr<Entity>> entities;
    entities.push_back(std::move(victim));
    CollisionEvents events;

    tileMap.queueTileHit(brick.x, brick.y, 32.f);
    tileMap.processPendingHits(entities,
                               TextureManager::getInstance(),
                               false,
                               &mario);

    assert(victimPtr->isDead());
    assert(victimPtr->isDying());
    assert(victimPtr->getBody()->GetLinearVelocity().y < 0.f);
    assert(mario.getScore() == ScoreRules::pointsFor(DefeatCause::BLOCK_BUMP));
    assert(events.blockKill == 1);
    assert(SoundManager::getInstance().getSoundPlayRequestCount("stomp") == 1);

    // Re-queueing the same hit cannot award the dead enemy a second time.
    tileMap.queueTileHit(brick.x, brick.y, 32.f);
    tileMap.processPendingHits(entities,
                               TextureManager::getInstance(),
                               false,
                               &mario);
    assert(events.blockKill == 1);
    assert(mario.getScore() == ScoreRules::pointsFor(DefeatCause::BLOCK_BUMP));
}

void runStarBrickCapability() {
    auto breakBrickFor = [](Mario& mario) {
        TileMap tileMap;
        assert(tileMap.loadFromFile("levels/level1.txt"));
        const auto brickTiles = tileMap.findTiles('B');
        assert(!brickTiles.empty());
        const sf::Vector2i brick = brickTiles.front();
        std::vector<std::unique_ptr<Entity>> entities;
        const bool hit = tileMap.hitTile(brick.x,
                                         brick.y,
                                         mario.canBreakBricks(),
                                         entities,
                                         nullptr);
        return hit && tileMap.getTileAt(brick.x, brick.y) == '.';
    };

    Mario smallMario;
    assert(!smallMario.canBreakBricks());
    smallMario.setStarInvincible(0.1f);
    assert(smallMario.canBreakBricks());
    assert(breakBrickFor(smallMario));
    smallMario.update(0.2f);
    assert(!smallMario.isStarInvincible());
    assert(!smallMario.canBreakBricks());

    Mario smallFireMario;
    smallFireMario.setMarioState(MarioState::FIRE_SMALL);
    assert(!smallFireMario.canBreakBricks());
    smallFireMario.setStarInvincible(0.1f);
    assert(smallFireMario.canBreakBricks());
    assert(breakBrickFor(smallFireMario));
    smallFireMario.update(0.2f);
    assert(!smallFireMario.canBreakBricks());
}

void runStar(bool marioFixtureFirst) {
    SoundManager::getInstance().resetDiagnosticCounters();
    b2World world({0.f, 0.f});
    TileMap tileMap;
    ContactListener listener(tileMap);
    world.SetContactListener(&listener);
    std::unique_ptr<Mario> mario;
    std::unique_ptr<Goomba> victim;
    if (marioFixtureFirst) {
        mario = std::make_unique<Mario>(sf::Vector2f(0.f, 0.f),
                                        sf::Vector2f(28.f, 30.f));
        mario->initPhysics(&world, b2_dynamicBody, {28.f, 30.f});
        victim = std::make_unique<Goomba>(sf::Vector2f(24.f, 0.f), &world);
    } else {
        victim = std::make_unique<Goomba>(sf::Vector2f(24.f, 0.f), &world);
        mario = std::make_unique<Mario>(sf::Vector2f(0.f, 0.f),
                                        sf::Vector2f(28.f, 30.f));
        mario->initPhysics(&world, b2_dynamicBody, {28.f, 30.f});
    }
    CollisionEvents events;
    mario->setStarInvincible(5.f);
    stepTwice(world);
    assert(victim->isDead());
    assert(mario->getScore() == ScoreRules::pointsFor(DefeatCause::STAR));
    assert(events.starKill == 1);
    assert(SoundManager::getInstance().getSoundPlayRequestCount("enemy_star") == 1);
}
} // namespace

int main() {
    SoundManager::getInstance(); // install the production SFX observer
    for (bool orderA : {true, false}) {
        runStomp(orderA);
        runSideHit(orderA);
        runDamageGracePassThrough(orderA);
        runIdleShellStaysStationary();
        runShell(orderA);
        runFireBall(orderA);
        runFireBallPassesDeadEnemy(orderA);
        runPiranhaFireBall(orderA);
        runStar(orderA);
    }
    runKoopaPitFallsPastPitWall();
    runBlockBumpDefeatsEnemy();
    runStarBrickCapability();
    runPiranhaFireRetractOnlyUpright();
    return 0;
}
