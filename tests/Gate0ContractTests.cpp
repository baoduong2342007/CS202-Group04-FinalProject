/**
 * @file Gate0ContractTests.cpp
 * @author TV1 (Dương)
 * @brief Gate 0 release-contract regression guards (s6_fix_plan_v2.md §2).
 *
 * This suite locks the Sprint 6 release contract that every other module builds
 * on top of:
 *   - `MarioState` is exactly { SMALL, SUPER, FIRE } — the abandoned small-fire
 *     variant must never return as a fourth enumerator. The static_asserts below
 *     make any re-introduction a compile-time failure.
 *   - Default character is Mario (`CharacterType::MARIO`); Luigi stays out of
 *     the release flow.
 *   - FireFlower ALWAYS ends in `MarioState::FIRE`, whatever the prior state.
 *   - Mushroom never downgrades an already-powered-up Mario.
 *   - All four release levels (1, 2, 3, 4) are loadable through the validator and
 *     the tileset, each with exactly one Mario spawn and one finish flag.
 *
 * NOTE: The FireBall active-limit guard (max 2, never 4) is a runtime property
 * of Level/b2World and is owned by TV3 (S6-TV3-19). It is asserted in the
 * integration phase once the unified single-API limit is merged; this file keeps
 * the interface guards that must hold from the very first day of Gate 0.
 */

#include <cassert>
#include <iostream>
#include <string>

#include <SFML/Graphics.hpp>

#include "core/LevelCatalog.h"
#include "entities/Mario.h"
#include "items/FireFlower.h"
#include "items/Mushroom.h"
#include "level/Level.h"
#include "level/TileMap.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "patterns/IObserver.h"

namespace {

class CompletionCounter final : public IObserver {
public:
    CompletionCounter() {
        EventBus::getInstance().subscribe(EventType::LEVEL_COMPLETED, this);
    }

    ~CompletionCounter() override {
        EventBus::getInstance().unsubscribe(EventType::LEVEL_COMPLETED, this);
    }

    void onNotify(EventType event) override {
        if (event == EventType::LEVEL_COMPLETED) {
            ++count;
        }
    }

    int count = 0;
};

// ===== Compile-time interface guards =====
// If the abandoned small-fire variant (or any other corrupt state) is
// re-introduced before one of these enumerators, THIS TRANSLATION UNIT STOPS
// COMPILING and CI fails.
static_assert(static_cast<int>(MarioState::SMALL) == 0,
              "Gate 0: MarioState must begin with SMALL == 0");
static_assert(static_cast<int>(MarioState::SUPER) == 1,
              "Gate 0: MarioState::SUPER must be the 2nd enumerator");
static_assert(static_cast<int>(MarioState::FIRE) == 2,
              "Gate 0: MarioState::FIRE must be the 3rd and final enumerator");

// Returns true when the given release level passes the validator and tileset load.
bool loadReleaseLevel(const std::string& filePath) {
    TileMap tileMap;
    return tileMap.loadFromFile(filePath);
}

// ===== Runtime contract guards =====

void testDefaultCharacterAndState() {
    std::cout << "[RUNNING] testDefaultCharacterAndState..." << std::endl;

    Mario mario;
    assert(mario.getMarioState() == MarioState::SMALL);
    // Gate 0: the default player character is Mario, not Luigi.
    assert(mario.getCharacterType() == CharacterType::MARIO);

    std::cout << "[PASSED] testDefaultCharacterAndState" << std::endl;
}

void testFireFlowerAlwaysGrantsFireFromSmall() {
    std::cout << "[RUNNING] testFireFlowerAlwaysGrantsFireFromSmall..." << std::endl;

    Mario mario; // starts SMALL
    FireFlower flower;
    flower.onCollect(mario);

    // A SMALL Mario picking up a FireFlower must end in FIRE directly
    // (exactly one result), never in a small-fire variant.
    assert(mario.getMarioState() == MarioState::FIRE);
    assert(mario.canShootFireBall());

    std::cout << "[PASSED] testFireFlowerAlwaysGrantsFireFromSmall" << std::endl;
}

void testFireFlowerAlwaysGrantsFireFromSuper() {
    std::cout << "[RUNNING] testFireFlowerAlwaysGrantsFireFromSuper..." << std::endl;

    Mario mario;
    mario.setMarioState(MarioState::SUPER);
    FireFlower flower;
    flower.onCollect(mario);

    assert(mario.getMarioState() == MarioState::FIRE);
    assert(mario.canShootFireBall());

    std::cout << "[PASSED] testFireFlowerAlwaysGrantsFireFromSuper" << std::endl;
}
void testMushroomPromotesAndNeverDowngrades() {
    std::cout << "[RUNNING] testMushroomPromotesAndNeverDowngrades..." << std::endl;

    // SMALL + Super Mushroom -> SUPER
    Mario small;
    Mushroom mushroom(MushroomType::SUPER);
    mushroom.onCollect(small);
    assert(small.getMarioState() == MarioState::SUPER);

    // SUPER + Super Mushroom -> stays SUPER (no downgrade)
    Mario super;
    super.setMarioState(MarioState::SUPER);
    Mushroom superMushroom(MushroomType::SUPER);
    superMushroom.onCollect(super);
    assert(super.getMarioState() == MarioState::SUPER);

    // FIRE + Super Mushroom -> stays FIRE (no downgrade)
    Mario fire;
    fire.setMarioState(MarioState::FIRE);
    Mushroom fireMushroom(MushroomType::SUPER);
    fireMushroom.onCollect(fire);
    assert(fire.getMarioState() == MarioState::FIRE);

    std::cout << "[PASSED] testMushroomPromotesAndNeverDowngrades" << std::endl;
}

void testReleaseLevelsAreLoadable() {
    std::cout << "[RUNNING] testReleaseLevelsAreLoadable..." << std::endl;

    // S6-TV1-14 dependency evidence: the four release levels must pass the
    // validator and load their tileset before PlayState can ever reach Win.
    assert(loadReleaseLevel("levels/level1.txt"));
    assert(loadReleaseLevel("levels/level2.txt"));
    assert(loadReleaseLevel("levels/level3.txt"));
    assert(loadReleaseLevel("levels/level4.txt"));

    std::cout << "[PASSED] testReleaseLevelsAreLoadable" << std::endl;
}

void testReleaseLevelMarkers() {
    std::cout << "[RUNNING] testReleaseLevelMarkers..." << std::endl;

    const std::string levelFiles[] = {
        "levels/level1.txt",
        "levels/level2.txt",
        "levels/level3.txt",
        "levels/level4.txt",
    };

    for (const std::string& filePath : levelFiles) {
        TileMap tileMap;
        assert(tileMap.loadFromFile(filePath));
        assert(tileMap.getWidth() > 0);
        assert(tileMap.getHeight() > 0);
        // Validator contract: exactly one Mario spawn and one finish per level.
        assert(tileMap.findTiles('M').size() == 1);
        assert(tileMap.findTiles('F').size() == 1);
    }

    TileMap castle;
    assert(castle.loadFromFile("levels/level4.txt"));
    assert(castle.getWidth() == 96);
    assert(castle.getHeight() == 16);
    assert(castle.findTiles('V').size() >= 7);
    assert(castle.isClimbable(60, 5));
    assert(!castle.isSolid(60, 5));

    std::cout << "[PASSED] testReleaseLevelMarkers" << std::endl;
}

#include "level/Level.h"

void testFireBallActiveLimitOfTwo() {
    std::cout << "[RUNNING] testFireBallActiveLimitOfTwo..." << std::endl;

    Level level;
    assert(loadReleaseLevel("levels/level1.txt"));
    assert(level.loadFromFile("levels/level1.txt"));
    Mario* mario = level.getMario();
    assert(mario != nullptr);

    // SMALL Mario cannot shoot fireballs
    assert(mario->getMarioState() == MarioState::SMALL);
    assert(!mario->canShootFireBall());
    assert(!level.requestFireBallShot(*mario));

    // Power up to FIRE state
    mario->setMarioState(MarioState::FIRE);
    assert(mario->canShootFireBall());

    // 1st shot -> succeeds
    assert(level.requestFireBallShot(*mario));

    // Fast-forward cooldown
    mario->update(0.3f);
    assert(mario->canShootFireBall());

    // 2nd shot -> succeeds
    assert(level.requestFireBallShot(*mario));

    mario->update(0.3f);
    assert(mario->canShootFireBall());

    // 3rd shot -> MUST FAIL because active count is 2 (hard limit of 2)
    assert(!level.requestFireBallShot(*mario));

    std::cout << "[PASSED] testFireBallActiveLimitOfTwo" << std::endl;
}

void testThemeWiringAndLevel4Spawns() {
    std::cout << "[RUNNING] testThemeWiringAndLevel4Spawns..." << std::endl;

    Level underwater;
    underwater.setTheme(LevelTheme::UNDERWATER);
    assert(underwater.loadFromFile("levels/level3.txt"));
    assert(underwater.getMario());
    assert(underwater.getMario()->isUnderwater());
    assert(underwater.getMario()->getBody());
    assert(underwater.getMario()->getBody()->GetLinearDamping() == 1.5f);

    Level castle;
    castle.setTheme(LevelTheme::CASTLE);
    assert(castle.loadFromFile("levels/level4.txt"));

    int enemyCount = 0;
    bool hasKoopa = false;
    bool hasPiranhaPlant = false;
    bool hasQuestionBlock = false;
    bool hasSpringboard = false;
    for (const auto& entity : castle.getEntities()) {
        assert(entity);
        enemyCount += entity->isEnemy() ? 1 : 0;
        hasKoopa = hasKoopa || entity->isKoopa();
        hasPiranhaPlant = hasPiranhaPlant || entity->isPiranhaPlant();
        hasQuestionBlock = hasQuestionBlock || entity->isQuestionBlock();
        hasSpringboard = hasSpringboard || entity->isSpringboard();
    }

    assert(enemyCount >= 3);
    assert(hasKoopa);
    assert(hasPiranhaPlant);
    assert(hasQuestionBlock);
    assert(hasSpringboard);

    std::cout << "[PASSED] testThemeWiringAndLevel4Spawns" << std::endl;
}

void testFlagAnimationChangesPixelsForEveryTheme() {
    std::cout << "[RUNNING] testFlagAnimationChangesPixelsForEveryTheme..." << std::endl;

    struct ThemeCase {
        const char* path;
        LevelTheme theme;
    };

    const ThemeCase cases[] = {
        {"levels/level1.txt", LevelTheme::OVERWORLD},
        {"levels/level2.txt", LevelTheme::UNDERGROUND},
        {"levels/level3.txt", LevelTheme::UNDERWATER},
        {"levels/level4.txt", LevelTheme::CASTLE},
    };

    for (const ThemeCase& testCase : cases) {
        TileMap tileMap;
        tileMap.setTheme(testCase.theme);
        assert(tileMap.loadFromFile(testCase.path));

        const sf::Vector2i finish = tileMap.findTiles('F').front();
        const sf::Vector2f finishPosition = TileMap::gridToWorldPosition(finish);

        sf::RenderTexture renderTexture;
        assert(renderTexture.resize({128u, 128u}));
        renderTexture.setView(sf::View(sf::FloatRect(
            {finishPosition.x - 32.0f, finishPosition.y - 32.0f},
            {128.0f, 128.0f})));

        renderTexture.clear(sf::Color::Transparent);
        tileMap.renderForeground(renderTexture);
        renderTexture.display();
        const sf::Image firstFrame = renderTexture.getTexture().copyToImage();

        tileMap.update(0.25f);
        renderTexture.clear(sf::Color::Transparent);
        tileMap.renderForeground(renderTexture);
        renderTexture.display();
        const sf::Image secondFrame = renderTexture.getTexture().copyToImage();

        bool changed = false;
        for (unsigned int y = 0; y < 128u && !changed; ++y) {
            for (unsigned int x = 0; x < 128u; ++x) {
                if (firstFrame.getPixel({x, y}) != secondFrame.getPixel({x, y})) {
                    changed = true;
                    break;
                }
            }
        }
        assert(changed);
    }

    std::cout << "[PASSED] testFlagAnimationChangesPixelsForEveryTheme" << std::endl;
}

void testLevel4FlagSequencePublishesOnce() {
    std::cout << "[RUNNING] testLevel4FlagSequencePublishesOnce..." << std::endl;

    Level level;
    level.setTheme(LevelTheme::CASTLE);
    assert(level.loadFromFile("levels/level4.txt"));

    CompletionCounter counter;
    const sf::Vector2i finish = level.getTileMap().findTiles('F').front();
    level.getMario()->setPosition(TileMap::gridToWorldPosition(finish));

    // Contact starts the sequence but must not complete the level immediately.
    level.update(0.0f);
    assert(level.getMario()->isFlagpoleSliding());
    assert(!level.isLevelCompleted());
    assert(counter.count == 0);

    // The long Level 4 pole is traversed at the fixed slide speed.
    for (int step = 0; step < 6 && !level.isLevelCompleted(); ++step) {
        level.update(1.0f);
    }

    assert(level.isLevelCompleted());
    assert(counter.count == 1);
    level.update(1.0f);
    assert(counter.count == 1);

    std::cout << "[PASSED] testLevel4FlagSequencePublishesOnce" << std::endl;
}

} // namespace

int main() {
    testDefaultCharacterAndState();
    testFireFlowerAlwaysGrantsFireFromSmall();
    testFireFlowerAlwaysGrantsFireFromSuper();
    testMushroomPromotesAndNeverDowngrades();
    testReleaseLevelsAreLoadable();
    testReleaseLevelMarkers();
    testFireBallActiveLimitOfTwo();
    testThemeWiringAndLevel4Spawns();
    testFlagAnimationChangesPixelsForEveryTheme();
    testLevel4FlagSequencePublishesOnce();

    std::cout << "All Gate0 contract tests passed successfully!" << std::endl;
    return 0;
}
