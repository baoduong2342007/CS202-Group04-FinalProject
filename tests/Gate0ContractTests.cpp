/**
 * @file Gate0ContractTests.cpp
 * @author TV1 (Dương)
 * @brief Gate 0 release-contract regression guards (s6_fix_plan_v2.md §2).
 *
 * This suite locks the Sprint 6 release contract that every other module builds
 * on top of:
 *   - `MarioState` is exactly { SMALL, SUPER, FIRE_SMALL, FIRE_SUPER }.
 *   - Default character is Mario (`CharacterType::MARIO`); Luigi stays out of
 *     the release flow.
 *   - FireFlower preserves the body tier: SMALL -> Small Fire, SUPER -> Super Fire.
 *   - Mushroom never downgrades an already-powered-up Mario.
 *   - The three release levels (1, 2, 3) are loadable through the validator and
 *     the tileset, each with exactly one Mario spawn and one finish flag.
 *
 * NOTE: The FireBall active-limit guard (max 2, never 4) is a runtime property
 * of Level/b2World and is owned by TV3 (S6-TV3-19). It is asserted in the
 * integration phase once the unified single-API limit is merged; this file keeps
 * the interface guards that must hold from the very first day of Gate 0.
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

#include <SFML/Graphics.hpp>

#include "core/LevelCatalog.h"
#include "entities/Mario.h"
#include "entities/QuestionBlock.h"
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

class BlockBumpCounter final : public IObserver {
public:
    BlockBumpCounter() {
        EventBus::getInstance().subscribe(EventType::BLOCK_BUMPED, this);
    }
    ~BlockBumpCounter() override {
        EventBus::getInstance().unsubscribe(EventType::BLOCK_BUMPED, this);
    }
    void onNotify(EventType event) override {
        if (event == EventType::BLOCK_BUMPED) ++count;
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
static_assert(static_cast<int>(MarioState::FIRE_SMALL) == 2,
              "Gate 0: MarioState::FIRE_SMALL must be the 3rd enumerator");
static_assert(static_cast<int>(MarioState::FIRE_SUPER) == 3,
              "Gate 0: MarioState::FIRE_SUPER must be the 4th and final enumerator");

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

void testFireFlowerPreservesBodyTier() {
    std::cout << "[RUNNING] testFireFlowerPreservesBodyTier..." << std::endl;

    Mario mario; // starts SMALL
    FireFlower flower;
    flower.onCollect(mario);

    // Small Mario becomes the explicit Small Fire state.
    assert(mario.getMarioState() == MarioState::FIRE_SMALL);
    assert(mario.getSize().y == 30.f);
    assert(mario.canShootFireBall());

    // Super Mario becomes the explicit Super Fire state.
    Mario superMario;
    superMario.setMarioState(MarioState::SUPER);
    FireFlower superFlower;
    superFlower.onCollect(superMario);
    assert(superMario.getMarioState() == MarioState::FIRE_SUPER);
    assert(superMario.getSize().y == 60.f);
    assert(superMario.canShootFireBall());

    std::cout << "[PASSED] testFireFlowerPreservesBodyTier" << std::endl;
}

void testFireFlowerGrantsFireFromSuper() {
    std::cout << "[RUNNING] testFireFlowerGrantsFireFromSuper..." << std::endl;

    Mario mario;
    mario.setMarioState(MarioState::SUPER);
    FireFlower flower;
    flower.onCollect(mario);

    assert(mario.getMarioState() == MarioState::FIRE_SUPER);
    assert(mario.getSize().y == 60.f);
    assert(mario.canShootFireBall());

    std::cout << "[PASSED] testFireFlowerGrantsFireFromSuper" << std::endl;
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

    // FIRE_SMALL + Super Mushroom -> FIRE_SUPER
    Mario fire;
    fire.setMarioState(MarioState::FIRE_SMALL);
    Mushroom fireMushroom(MushroomType::SUPER);
    fireMushroom.onCollect(fire);
    assert(fire.getMarioState() == MarioState::FIRE_SUPER);
    assert(fire.getSize().y == 60.f);
    assert(fire.canShootFireBall());

    // FIRE_SUPER + Super Mushroom -> stays FIRE_SUPER
    Mario superFire;
    superFire.setMarioState(MarioState::FIRE_SUPER);
    Mushroom superFireMushroom(MushroomType::SUPER);
    superFireMushroom.onCollect(superFire);
    assert(superFire.getMarioState() == MarioState::FIRE_SUPER);

    std::cout << "[PASSED] testMushroomPromotesAndNeverDowngrades" << std::endl;
}

void testReleaseLevelsAreLoadable() {
    std::cout << "[RUNNING] testReleaseLevelsAreLoadable..." << std::endl;

    // S6-TV1-14 dependency evidence: the three release levels must pass the
    // validator and load their tileset before PlayState can ever reach Win.
    assert(loadReleaseLevel("levels/level1.txt"));
    assert(loadReleaseLevel("levels/level2.txt"));
    assert(loadReleaseLevel("levels/level3.txt"));

    std::cout << "[PASSED] testReleaseLevelsAreLoadable" << std::endl;
}

void testReleaseLevelMarkers() {
    std::cout << "[RUNNING] testReleaseLevelMarkers..." << std::endl;

    const std::string levelFiles[] = {
        "levels/level1.txt",
        "levels/level2.txt",
        "levels/level3.txt",
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
    assert(castle.loadFromFile("levels/level3.txt"));
    assert(castle.getWidth() == 96);
    assert(castle.getHeight() == 16);
    assert(castle.findTiles('V').size() >= 7);
    assert(castle.isClimbable(60, 5));
    assert(!castle.isSolid(60, 5));

    std::cout << "[PASSED] testReleaseLevelMarkers" << std::endl;
}

void testAllLevelFlagMarkers() {
    std::cout << "[RUNNING] testAllLevelFlagMarkers..." << std::endl;

    struct LevelCase {
        const char* path;
        LevelTheme theme;
    };

    const LevelCase cases[] = {
        {"levels/level0.txt", LevelTheme::OVERWORLD},
        {"levels/level1.txt", LevelTheme::OVERWORLD},
        {"levels/level2.txt", LevelTheme::UNDERGROUND},
        {"levels/level3.txt", LevelTheme::CASTLE},
        {"levels/level4.txt", LevelTheme::CASTLE},
    };

    for (const LevelCase& testCase : cases) {
        TileMap tileMap;
        tileMap.setTheme(testCase.theme);
        assert(tileMap.loadFromFile(testCase.path));

        const auto tops = tileMap.findTiles('T');
        const auto finishes = tileMap.findTiles('F');
        assert(tops.size() == 1);
        assert(finishes.size() == 1);
        assert(tops.front().x == finishes.front().x);
        assert(tops.front().y + 1 == finishes.front().y);

        int poleRow = finishes.front().y + 1;
        int poleTiles = 0;
        while (poleRow < static_cast<int>(tileMap.getHeight()) &&
               tileMap.getTileAt(finishes.front().x, poleRow) == '|') {
            ++poleTiles;
            ++poleRow;
        }

        assert(poleTiles > 0);
        assert(poleRow < static_cast<int>(tileMap.getHeight()));
        assert(tileMap.isSolid(finishes.front().x, poleRow));
    }

    std::cout << "[PASSED] testAllLevelFlagMarkers" << std::endl;
}

void testRandomQuestionBlocksAndItemRoutes() {
    assert(QuestionBlock::chooseRandomContent(0) == QuestionBlockContent::COIN);
    assert(QuestionBlock::chooseRandomContent(139) == QuestionBlockContent::COIN);
    assert(QuestionBlock::chooseRandomContent(140) ==
           QuestionBlockContent::SUPER_MUSHROOM);
    assert(QuestionBlock::chooseRandomContent(169) ==
           QuestionBlockContent::SUPER_MUSHROOM);
    assert(QuestionBlock::chooseRandomContent(170) ==
           QuestionBlockContent::FIRE_FLOWER);
    assert(QuestionBlock::chooseRandomContent(199) ==
           QuestionBlockContent::FIRE_FLOWER);

    BlockBumpCounter events;
    Mario mario;
    QuestionBlock block({0.f, 0.f}, nullptr);
    block.onHit(mario);
    block.onHit(mario);
    assert(block.getContent() == QuestionBlockContent::COIN ||
           block.getContent() == QuestionBlockContent::SUPER_MUSHROOM ||
           block.getContent() == QuestionBlockContent::FIRE_FLOWER);
    assert(events.count == 1);

    // An explicit 'f' block always exposes a Fire Flower. The collected item
    // preserves Small/Super body size through Mario::powerUp().
    Mario explicitSmallMario;
    QuestionBlock explicitFlowerForSmall(
        {0.f, 0.f}, nullptr, QuestionBlockContent::FIRE_FLOWER);
    explicitFlowerForSmall.onHit(explicitSmallMario);
    assert(explicitFlowerForSmall.getContent() ==
           QuestionBlockContent::FIRE_FLOWER);

    Mario explicitSuperMario;
    explicitSuperMario.setMarioState(MarioState::SUPER);
    QuestionBlock explicitFlowerForSuper(
        {0.f, 0.f}, nullptr, QuestionBlockContent::FIRE_FLOWER);
    explicitFlowerForSuper.onHit(explicitSuperMario);
    assert(explicitFlowerForSuper.getContent() ==
           QuestionBlockContent::FIRE_FLOWER);

    const auto hasUsableBlockRoute = [](const TileMap& tileMap,
                                        char symbol) {
        for (const sf::Vector2i& tile : tileMap.findTiles(symbol)) {
            if (tile.y + 2 >= static_cast<int>(tileMap.getHeight()) ||
                tileMap.isSolid(tile.x, tile.y + 1) ||
                tileMap.isSolid(tile.x, tile.y + 2)) {
                continue;
            }

            // A floor three to six tiles below leaves enough headroom for
            // Mario to stand and jump into the block from underneath.
            for (int distance = 3; distance <= 6; ++distance) {
                const int floorRow = tile.y + distance;
                if (floorRow < static_cast<int>(tileMap.getHeight()) &&
                    tileMap.isSolid(tile.x, floorRow)) {
                    return true;
                }
            }
        }
        return false;
    };

    for (const LevelDefinition& definition : LevelCatalog::getAll()) {
        TileMap tileMap;
        assert(tileMap.loadFromFile(definition.filePath));
        assert(hasUsableBlockRoute(tileMap, '?'));
        assert(hasUsableBlockRoute(tileMap, 'f'));
        assert(hasUsableBlockRoute(tileMap, 'U'));
        assert(hasUsableBlockRoute(tileMap, 'O'));
    }
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

    // Power up to FIRE_SMALL state
    mario->setMarioState(MarioState::FIRE_SMALL);
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

void testThemeWiringAndLevel3Spawns() {
    std::cout << "[RUNNING] testThemeWiringAndLevel3Spawns..." << std::endl;
    Level castle;
    castle.setTheme(LevelTheme::CASTLE);
    assert(castle.loadFromFile("levels/level3.txt"));

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

    std::cout << "[PASSED] testThemeWiringAndLevel3Spawns" << std::endl;
}

void testFlagAnimationChangesPixelsForEveryTheme() {
    std::cout << "[RUNNING] testFlagAnimationChangesPixelsForEveryTheme..." << std::endl;

    struct ThemeCase {
        const char* path;
        LevelTheme theme;
    };

    const ThemeCase cases[] = {
        {"levels/level0.txt", LevelTheme::OVERWORLD},
        {"levels/level1.txt", LevelTheme::OVERWORLD},
        {"levels/level2.txt", LevelTheme::UNDERGROUND},
        {"levels/level3.txt", LevelTheme::CASTLE},
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
        tileMap.renderFlags(renderTexture);
        tileMap.renderForeground(renderTexture);
        renderTexture.display();
        const sf::Image firstFrame = renderTexture.getTexture().copyToImage();

        tileMap.update(0.25f);
        renderTexture.clear(sf::Color::Transparent);
        tileMap.renderFlags(renderTexture);
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

        // The visual drop is independent from collision geometry and is
        // clamped to the validated pole body by TileMap.
        tileMap.setFlagDropDistance(32.0f);
        renderTexture.clear(sf::Color::Transparent);
        tileMap.renderFlags(renderTexture);
        tileMap.renderForeground(renderTexture);
        renderTexture.display();
        const sf::Image loweredFrame = renderTexture.getTexture().copyToImage();

        bool lowered = false;
        for (unsigned int y = 0; y < 128u && !lowered; ++y) {
            for (unsigned int x = 0; x < 128u; ++x) {
                if (secondFrame.getPixel({x, y}) != loweredFrame.getPixel({x, y})) {
                    lowered = true;
                    break;
                }
            }
        }
        assert(lowered);
    }

    std::cout << "[PASSED] testFlagAnimationChangesPixelsForEveryTheme" << std::endl;
}

void testLevel3FlagSequencePublishesOnce() {
    std::cout << "[RUNNING] testLevel3FlagSequencePublishesOnce..." << std::endl;

    Level level;
    level.setTheme(LevelTheme::CASTLE);
    assert(level.loadFromFile("levels/level3.txt"));

    CompletionCounter counter;
    const sf::Vector2i finish = level.getTileMap().findTiles('F').front();
    const sf::Vector2f startPosition =
        TileMap::gridToWorldPosition(finish) + sf::Vector2f(7.0f, 8.0f);
    level.getMario()->setPosition(startPosition);

    // Contact starts the sequence but must not complete the level immediately.
    level.update(0.0f);
    assert(level.getMario()->isFlagpoleSliding());
    assert(level.getMario()->getPosition() == startPosition);
    assert(level.getTileMap().getFlagDropDistance() == 0.0f);
    assert(!level.isLevelCompleted());
    assert(counter.count == 0);

    const float previousMarioY = level.getMario()->getPosition().y;
    const float previousFlagDrop = level.getTileMap().getFlagDropDistance();
    level.update(0.25f);
    const float marioDisplacement =
        level.getMario()->getPosition().y - previousMarioY;
    assert(marioDisplacement > 0.0f);
    assert(std::abs(level.getTileMap().getFlagDropDistance() -
                    (previousFlagDrop + marioDisplacement)) < 0.1f);

    // The Castle pole is traversed at the fixed slide speed.
    for (int step = 0; step < 6 && !level.isLevelCompleted(); ++step) {
        level.update(1.0f);
    }

    assert(level.isLevelCompleted());
    assert(counter.count == 1);
    level.update(1.0f);
    assert(counter.count == 1);

    std::cout << "[PASSED] testLevel3FlagSequencePublishesOnce" << std::endl;
}

} // namespace

int main() {
    testDefaultCharacterAndState();
    testFireFlowerPreservesBodyTier();
    testFireFlowerGrantsFireFromSuper();
    testMushroomPromotesAndNeverDowngrades();
    testReleaseLevelsAreLoadable();
    testReleaseLevelMarkers();
    testAllLevelFlagMarkers();
    testRandomQuestionBlocksAndItemRoutes();
    testFireBallActiveLimitOfTwo();
    testThemeWiringAndLevel3Spawns();
    testFlagAnimationChangesPixelsForEveryTheme();
    testLevel3FlagSequencePublishesOnce();

    std::cout << "All Gate0 contract tests passed successfully!" << std::endl;
    return 0;
}
