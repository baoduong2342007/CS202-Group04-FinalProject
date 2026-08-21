/**
 * @file P2GameplayInterfaceTests.cpp
 * @brief Deterministic public-interface probes for the P2 gameplay audit.
 * @note These tests observe Level-owned entities through EntityView only;
 *       no production container or private-state seams are used.
 */

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <box2d/box2d.h>

#include "core/LevelCatalog.h"
#include "entities/BulletBill.h"
#include "entities/BulletBillLauncher.h"
#include "entities/Enemy.h"
#include "entities/Lakitu.h"
#include "entities/Mario.h"
#include "entities/PiranhaPlant.h"
#include "entities/FireBall.h"
#include "items/Mushroom.h"
#include "items/Star.h"
#include "level/EntityView.h"
#include "level/Level.h"
#include "level/TileMap.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "patterns/IObserver.h"

namespace {

constexpr float FRAME_DT = 1.0f / 60.0f;
constexpr float TILE_SIZE = 32.0f;

class EventCounter final : public IObserver {
public:
    EventCounter() {
        EventBus& bus = EventBus::getInstance();
        m_subscriptions.emplace_back(
            bus.subscribe(EventType::PLAYER_POWER_UP, this));
        m_subscriptions.emplace_back(
            bus.subscribe(EventType::PLAYER_STAR_COLLECTED, this));
        m_subscriptions.emplace_back(
            bus.subscribe(EventType::ONE_UP_COLLECTED, this));
    }

    void onNotify(const GameEvent& event) override {
        switch (event.type) {
        case EventType::PLAYER_POWER_UP:
            ++powerUp;
            break;
        case EventType::PLAYER_STAR_COLLECTED:
            ++star;
            break;
        case EventType::ONE_UP_COLLECTED:
            ++oneUp;
            break;
        default:
            break;
        }
    }

    int powerUp = 0;
    int star = 0;
    int oneUp = 0;

private:
    std::vector<Subscription> m_subscriptions;
};

std::size_t countSubtype(const EntityView& view,
                         Entity::EntitySubtype subtype) {
    return view.count(subtype);
}

bool isInMapBounds(const Entity& entity,
                   float width,
                   float height,
                   float margin = 64.0f) {
    const sf::FloatRect bounds = entity.getBoundingBox();
    return bounds.position.x >= -margin &&
           bounds.position.y >= -margin &&
           bounds.position.x + bounds.size.x <= width + margin &&
           bounds.position.y + bounds.size.y <= height + margin;
}

Entity::EntitySubtype subtypeForStaticCode(char code) {
    switch (code) {
    case 'G': return Entity::EntitySubtype::GOOMBA;
    case 'K': return Entity::EntitySubtype::KOOPA;
    case 'p':
    case 'q': return Entity::EntitySubtype::PIRANHA_PLANT;
    case 'b': return Entity::EntitySubtype::BUZZY_BEETLE;
    case 'k': return Entity::EntitySubtype::RED_KOOPA;
    case 'y':
    case 'd': return Entity::EntitySubtype::PARATROOPA;
    case 'l': return Entity::EntitySubtype::BLOOPER;
    case 'P': return Entity::EntitySubtype::PODOBOO;
    case 't': return Entity::EntitySubtype::LAKITU;
    case 's': return Entity::EntitySubtype::SPINY;
    case 'n': return Entity::EntitySubtype::HAMMER_BRO;
    case 'X': return Entity::EntitySubtype::BOWSER;
    case 'e': return Entity::EntitySubtype::FIREBAR;
    case 'A': return Entity::EntitySubtype::BOWSER_AXE;
    case 'N': return Entity::EntitySubtype::TOAD;
    case 'J': return Entity::EntitySubtype::SPRINGBOARD;
    case 'C': return Entity::EntitySubtype::COIN;
    case '?':
    case 'U':
    case 'u':
    case 'O':
    case 'o':
    case 'f':
    case 'h': return Entity::EntitySubtype::QUESTION_BLOCK;
    default: return Entity::EntitySubtype::UNKNOWN;
    }
}

void assertStaticSpawnsHaveRuntimeEntity(const TileMap& map,
                                         const EntityView& view) {
    constexpr char STATIC_SPAWN_CODES[] = {
        'G', 'K', 'p', 'q', 'b', 'k', 'y', 'd', 'l', 'P', 't', 's', 'n',
        'X', 'e', 'A', 'N', 'J', 'C', '?', 'U', 'u', 'O', 'o', 'f', 'h'};

    for (char code : STATIC_SPAWN_CODES) {
        const auto positions = map.findTiles(code);
        if (positions.empty()) {
            continue;
        }

        const Entity::EntitySubtype subtype = subtypeForStaticCode(code);
        assert(subtype != Entity::EntitySubtype::UNKNOWN);
        assert(view.count(subtype) >= positions.size());
    }
}

void setPlayerProbePosition(Mario& mario, float x, float y = -128.0f) {
    mario.setPosition({x, y});
    mario.setVelocity({0.0f, 0.0f});
    mario.setPitThreshold(100000.0f);
}

void testReleaseLevelRuntimeSweep() {
    std::cout << "[RUNNING] testReleaseLevelRuntimeSweep..." << std::endl;

    for (const LevelDefinition& definition : LevelCatalog::getAll()) {
        TileMap staticMap;
        assert(staticMap.loadFromFile(definition.filePath));

        Level level;
        level.setTheme(definition.initialTheme);
        level.setCameraVerticalMode(definition.cameraMode);
        assert(level.loadFromFile(definition.filePath));

        const float levelWidth =
            static_cast<float>(staticMap.getWidth()) * TILE_SIZE;
        const float levelHeight =
            static_cast<float>(staticMap.getHeight()) * TILE_SIZE;
        EntityView initial = level.getEntities();
        assert(!initial.empty());
        assertStaticSpawnsHaveRuntimeEntity(staticMap, initial);

        std::map<Entity::EntitySubtype, std::size_t> baselineCounts;
        std::size_t baselineEnemies = 0;
        for (const Entity& entity : initial) {
            assert(isInMapBounds(entity, levelWidth, levelHeight));
            ++baselineCounts[entity.getSubtype()];
            if (const auto* enemy = dynamic_cast<const Enemy*>(&entity)) {
                (void)enemy;
                ++baselineEnemies;
            }
        }
        assert(baselineEnemies > 0);

        Mario* mario = level.getMario();
        assert(mario != nullptr);
        const float startCameraX =
            level.getCamera().getView().getCenter().x;

        // Move the public player position through the map in bounded hops.
        // The elevated probe Y avoids synthetic enemy/item contact while the
        // camera and activation gates still observe each reachable X area.
        // Keep successive targets within half a logical camera width so no
        // static enemy area is skipped between activation windows.
        const int hopCount = std::max(
            12, static_cast<int>(std::ceil(levelWidth / 320.0f)));
        for (int hop = 0; hop <= hopCount; ++hop) {
            const float fraction = static_cast<float>(hop) /
                                   static_cast<float>(hopCount);
            const float targetX =
                std::clamp(fraction * (levelWidth - 32.0f), 0.0f,
                           levelWidth - 32.0f);
            setPlayerProbePosition(*mario, targetX);
            for (int frame = 0; frame < 3; ++frame) {
                level.update(FRAME_DT);
                const EntityView frameView = level.getEntities();
                for (const Entity& entity : frameView) {
                    assert(isInMapBounds(entity, levelWidth, levelHeight));
                }
            }
        }

        const EntityView finalView = level.getEntities();
        for (const auto& [subtype, count] : baselineCounts) {
            // Static map entities must still be observable after the runtime
            // sweep. Dynamic spawner children may increase the count.
            assert(finalView.count(subtype) >= count);
        }
        assert(level.getCamera().getView().getCenter().x > startCameraX);

        std::size_t activatedEnemies = 0;
        for (const Entity& entity : finalView) {
            if (const auto* enemy = dynamic_cast<const Enemy*>(&entity)) {
                if (enemy->isActivated()) {
                    ++activatedEnemies;
                }
            }
        }
        if (activatedEnemies < baselineEnemies) {
            std::cerr << "[P2-E1] level=" << definition.filePath
                      << " baselineEnemies=" << baselineEnemies
                      << " activatedEnemies=" << activatedEnemies
                      << " finalEntities=" << finalView.size()
                      << " cameraX="
                      << level.getCamera().getView().getCenter().x << '\n';
            for (const auto& [subtype, count] : baselineCounts) {
                std::cerr << "  subtype=" << static_cast<int>(subtype)
                          << " baseline=" << count
                          << " final=" << finalView.count(subtype) << '\n';
            }
            for (const Entity& entity : finalView) {
                if (const auto* enemy = dynamic_cast<const Enemy*>(&entity)) {
                    if (!enemy->isActivated()) {
                        std::cerr << "  unactivated subtype="
                                  << static_cast<int>(entity.getSubtype())
                                  << " x=" << entity.getPosition().x
                                  << " y=" << entity.getPosition().y << '\n';
                    }
                }
            }
        }
        assert(activatedEnemies >= baselineEnemies);
    }

    std::cout << "[PASSED] testReleaseLevelRuntimeSweep" << std::endl;
}

void testLevelFireballCleanupAndLakituSpawnerOutput() {
    std::cout << "[RUNNING] testLevelFireballCleanupAndLakituSpawnerOutput..."
              << std::endl;

    Level level;
    assert(level.loadFromFile("levels/level1.txt"));
    Mario* mario = level.getMario();
    assert(mario != nullptr);
    mario->setMarioState(MarioState::FIRE_SMALL);
    mario->setFacingDirection(Direction::RIGHT);
    const float levelWidth =
        static_cast<float>(level.getTileMap().getWidth()) * TILE_SIZE;
    setPlayerProbePosition(*mario, levelWidth - 96.0f, -128.0f);

    assert(level.requestFireBallShot(*mario));
    EntityView withFireball = level.getEntities();
    assert(countSubtype(withFireball, Entity::EntitySubtype::FIRE_BALL) == 1);

    // The shot starts near the right edge and must be removed through the
    // normal Level lifecycle after crossing the public bounds gate.
    for (int frame = 0; frame < 120; ++frame) {
        level.update(FRAME_DT);
    }
    const EntityView afterFireball = level.getEntities();
    assert(countSubtype(afterFireball, Entity::EntitySubtype::FIRE_BALL) == 0);

    // Lakitu is a configured level spawner. Teleporting Mario into its
    // reachable horizontal area activates the public Level update path;
    // after its deterministic cruise/hide/throw cadence, an egg is adopted.
    const EntityView beforeSpawner = level.getEntities();
    const Entity* lakituEntity =
        beforeSpawner.find(Entity::EntitySubtype::LAKITU);
    assert(lakituEntity != nullptr);
    const float lakituX = lakituEntity->getPosition().x;
    setPlayerProbePosition(*mario, lakituX, -128.0f);

    bool sawEgg = false;
    bool sawSpiny = false;
    for (int frame = 0; frame < 100; ++frame) {
        setPlayerProbePosition(*mario, lakituX, -128.0f);
        level.update(0.1f);
        const EntityView view = level.getEntities();
        sawEgg = sawEgg ||
                 view.count(Entity::EntitySubtype::SPINY_EGG) > 0;
        sawSpiny = sawSpiny ||
                   view.count(Entity::EntitySubtype::SPINY) > 1;
        if (sawEgg && sawSpiny) {
            break;
        }
    }
    assert(sawEgg);
    assert(sawSpiny);

    std::cout << "[PASSED] testLevelFireballCleanupAndLakituSpawnerOutput"
              << std::endl;
}

void testCollectibleIdempotence() {
    std::cout << "[RUNNING] testCollectibleIdempotence..." << std::endl;

    b2World world(b2Vec2(0.0f, 0.0f));
    Mario mario({100.0f, 100.0f}, {32.0f, 32.0f});
    mario.initPhysics(&world, b2_dynamicBody, {32.0f, 32.0f});
    EventCounter events;

    Star star({100.0f, 100.0f}, &world);
    assert(star.checkOverlap(mario));
    const int starScore = mario.getScore();
    star.onCollect(mario);
    const int scoreAfterFirstStar = mario.getScore();
    assert(star.isCollected());
    assert(mario.isStarInvincible());
    star.onCollect(mario);
    assert(mario.getScore() == scoreAfterFirstStar);
    assert(scoreAfterFirstStar > starScore);
    assert(events.star == 1);
    assert(events.powerUp == 1);

    Mushroom oneUp({100.0f, 100.0f}, &world, MushroomType::ONE_UP);
    const int livesBefore = mario.getLives();
    const int scoreBefore = mario.getScore();
    assert(oneUp.checkOverlap(mario));
    oneUp.onCollect(mario);
    assert(oneUp.isCollected());
    assert(mario.getLives() == livesBefore + 1);
    const int scoreAfterOneUp = mario.getScore();
    assert(scoreAfterOneUp > scoreBefore);
    oneUp.onCollect(mario);
    assert(mario.getLives() == livesBefore + 1);
    assert(mario.getScore() == scoreAfterOneUp);
    assert(events.oneUp == 1);

    std::cout << "[PASSED] testCollectibleIdempotence" << std::endl;
}

void advanceEntity(Entity& entity, float seconds, float dt = 0.1f) {
    for (float elapsed = 0.0f; elapsed < seconds; elapsed += dt) {
        entity.update(dt);
    }
}

void testPiranhaProximityGuard() {
    std::cout << "[RUNNING] testPiranhaProximityGuard..." << std::endl;

    b2World world(b2Vec2(0.0f, 0.0f));
    PiranhaPlant farPlant({100.0f, 300.0f}, &world);
    farPlant.updateMarioProximity({1000.0f, 300.0f});
    advanceEntity(farPlant, 1.6f);
    assert(farPlant.getState() == PiranhaPlant::State::EMERGING);

    PiranhaPlant nearPlant({100.0f, 300.0f}, &world);
    const float nearInitialY = nearPlant.getPosition().y;
    nearPlant.updateMarioProximity({100.0f, 300.0f});
    advanceEntity(nearPlant, 3.0f);
    assert(nearPlant.getState() == PiranhaPlant::State::WAITING_BOTTOM);
    const float nearBaseY = nearPlant.getPosition().y;
    if (std::abs(nearBaseY - nearInitialY) >= 0.01f) {
        std::cerr << "[P2-A1] near plant y=" << nearBaseY << '\n';
    }
    assert(std::abs(nearBaseY - nearInitialY) < 0.01f);

    // Moving the target away re-arms emergence through the same public seam.
    nearPlant.updateMarioProximity({1000.0f, 300.0f});
    advanceEntity(nearPlant, 1.6f);
    assert(nearPlant.getState() == PiranhaPlant::State::EMERGING ||
           nearPlant.getState() == PiranhaPlant::State::WAITING_TOP);

    std::cout << "[PASSED] testPiranhaProximityGuard" << std::endl;
}

class TempCampaignMap final {
public:
    TempCampaignMap() {
        const auto stamp = std::chrono::high_resolution_clock::now()
                               .time_since_epoch().count();
        m_path = std::filesystem::temp_directory_path() /
                 ("p2_launcher_probe_" + std::to_string(stamp) + ".txt");

        constexpr int width = 80;
        std::vector<std::string> rows(15, std::string(width, '.'));
        rows[7][30] = 'D';
        rows[10][10] = 'M';
        rows[10][70] = 'N';
        rows[12] = std::string(width, '0');
        rows[13] = std::string(width, '0');
        rows[14] = std::string(width, '0');

        std::ofstream output(m_path);
        assert(output.is_open());
        for (const std::string& row : rows) {
            output << row << '\n';
        }
    }

    ~TempCampaignMap() {
        std::error_code ignored;
        std::filesystem::remove(m_path, ignored);
    }

    const std::filesystem::path& path() const { return m_path; }

private:
    std::filesystem::path m_path;
};

bool testCoopRetargetingAndLauncherNearestPlayer() {
    std::cout << "[RUNNING] testCoopRetargetingAndLauncherNearestPlayer..."
              << std::endl;

    Level coop;
    assert(coop.loadFromFile("levels/level1.txt",
                             CharacterType::MARIO,
                             CharacterType::LUIGI));
    const EntityView initial = coop.getEntities();
    const Entity* lakituEntity =
        initial.find(Entity::EntitySubtype::LAKITU);
    assert(lakituEntity != nullptr);
    const float lakituX = lakituEntity->getPosition().x;

    Mario* playerOne = coop.getMario();
    Mario* playerTwo = coop.getMario2();
    assert(playerOne != nullptr && playerTwo != nullptr);

    // Player two is the nearer target on the right, then on the left. A
    // genuine homing response must change sign when the nearest player swaps.
    setPlayerProbePosition(*playerOne, lakituX - 128.0f);
    setPlayerProbePosition(*playerTwo, lakituX + 64.0f);
    coop.update(0.1f);
    setPlayerProbePosition(*playerOne, lakituX - 128.0f);
    setPlayerProbePosition(*playerTwo, lakituX + 64.0f);
    coop.update(0.1f);
    const EntityView rightTargetView = coop.getEntities();
    const auto* lakituRight = dynamic_cast<const Lakitu*>(
        rightTargetView.find(Entity::EntitySubtype::LAKITU));
    assert(lakituRight != nullptr);
    assert(lakituRight->isActivated());
    assert(lakituRight->getVelocity().x > 0.0f);

    setPlayerProbePosition(*playerOne, lakituX + 128.0f);
    setPlayerProbePosition(*playerTwo, lakituX - 64.0f);
    coop.update(0.1f);
    const EntityView leftTargetView = coop.getEntities();
    const auto* lakituLeft = dynamic_cast<const Lakitu*>(
        leftTargetView.find(Entity::EntitySubtype::LAKITU));
    assert(lakituLeft != nullptr);
    assert(lakituLeft->getVelocity().x < 0.0f);

    // A D launcher in a temporary, validator-compliant campaign map makes
    // the player-one-only forwarding defect observable through Level. Player
    // two is nearer on the right, so the adopted Bullet Bill must fly right.
    TempCampaignMap fixture;
    Level launcherLevel;
    assert(launcherLevel.loadFromFile(fixture.path().string(),
                                      CharacterType::MARIO,
                                      CharacterType::LUIGI));
    const EntityView launchEntities = launcherLevel.getEntities();
    const Entity* launcherEntity =
        launchEntities.find(Entity::EntitySubtype::BULLET_BILL_LAUNCHER);
    assert(launcherEntity != nullptr);
    const float launcherX = launcherEntity->getPosition().x;
    Mario* launcherPlayerOne = launcherLevel.getMario();
    Mario* launcherPlayerTwo = launcherLevel.getMario2();
    assert(launcherPlayerOne != nullptr && launcherPlayerTwo != nullptr);
    setPlayerProbePosition(*launcherPlayerOne, launcherX - 128.0f, 100.0f);
    setPlayerProbePosition(*launcherPlayerTwo, launcherX + 64.0f, 100.0f);

    const BulletBill* observedBullet = nullptr;
    for (int frame = 0; frame < 45 && observedBullet == nullptr; ++frame) {
        setPlayerProbePosition(*launcherPlayerOne, launcherX - 128.0f, 100.0f);
        setPlayerProbePosition(*launcherPlayerTwo, launcherX + 64.0f, 100.0f);
        launcherLevel.update(0.1f);
        const EntityView view = launcherLevel.getEntities();
        if (const Entity* bullet = view.find(Entity::EntitySubtype::BULLET_BILL)) {
            observedBullet = dynamic_cast<const BulletBill*>(bullet);
        }
    }
    assert(observedBullet != nullptr);
    if (observedBullet->getDirection() != Direction::RIGHT) {
        std::cerr << "[P2-A2] launcher bullet direction="
                  << (observedBullet->getDirection() == Direction::LEFT
                          ? "LEFT"
                          : "RIGHT")
                  << " while player two is nearer on the right\n";
        return false;
    }
    assert(observedBullet->getDirection() == Direction::RIGHT);

    // Let the launcher cadence elapse again after the players swap sides.
    // The second adopted bullet must use the new nearest target rather than
    // the target cached for the previous shot.
    for (int frame = 0; frame < 35; ++frame) {
        setPlayerProbePosition(*launcherPlayerOne, launcherX - 64.0f, 100.0f);
        setPlayerProbePosition(*launcherPlayerTwo, launcherX + 128.0f, 100.0f);
        launcherLevel.update(0.1f);
    }
    const EntityView swappedView = launcherLevel.getEntities();
    assert(swappedView.count(Entity::EntitySubtype::BULLET_BILL) >= 2);
    const BulletBill* retargetedBullet = nullptr;
    for (const Entity& entity : swappedView) {
        if (entity.getSubtype() == Entity::EntitySubtype::BULLET_BILL) {
            retargetedBullet = dynamic_cast<const BulletBill*>(&entity);
        }
    }
    assert(retargetedBullet != nullptr);
    assert(retargetedBullet->getDirection() == Direction::LEFT);

    // Single-player remains deterministic: with only player one present on
    // the left, the launcher still fires one leftward Bullet Bill.
    Level singleLauncherLevel;
    assert(singleLauncherLevel.loadFromFile(fixture.path().string(),
                                             CharacterType::MARIO));
    const EntityView singleInitialView = singleLauncherLevel.getEntities();
    const Entity* singleLauncherEntity =
        singleInitialView.find(Entity::EntitySubtype::BULLET_BILL_LAUNCHER);
    assert(singleLauncherEntity != nullptr);
    const float singleLauncherX = singleLauncherEntity->getPosition().x;
    Mario* singlePlayer = singleLauncherLevel.getMario();
    assert(singlePlayer != nullptr);
    assert(singleLauncherLevel.getMario2() == nullptr);
    for (int frame = 0; frame < 45; ++frame) {
        setPlayerProbePosition(*singlePlayer, singleLauncherX - 64.0f, 100.0f);
        singleLauncherLevel.update(0.1f);
    }
    const EntityView singleView = singleLauncherLevel.getEntities();
    const Entity* singleBulletEntity =
        singleView.find(Entity::EntitySubtype::BULLET_BILL);
    const auto* singleBullet =
        dynamic_cast<const BulletBill*>(singleBulletEntity);
    assert(singleBullet != nullptr);
    assert(singleBullet->getDirection() == Direction::LEFT);

    std::cout << "[PASSED] testCoopRetargetingAndLauncherNearestPlayer"
              << std::endl;
    return true;
}

} // namespace

int main() {
    testReleaseLevelRuntimeSweep();
    testLevelFireballCleanupAndLakituSpawnerOutput();
    testCollectibleIdempotence();
    testPiranhaProximityGuard();
    const bool launcherNearestPlayerPassed =
        testCoopRetargetingAndLauncherNearestPlayer();
    if (!launcherNearestPlayerPassed) {
        std::cerr << "P2 gameplay interface tests failed." << std::endl;
        return 1;
    }
    std::cout << "All P2 gameplay interface tests passed successfully!"
              << std::endl;
    return 0;
}
