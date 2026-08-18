/**
 * @file Gate0ContractTests.cpp
 * @author TV1 (Dương)
 * @brief Gate 0 release-contract regression guards (s6_fix_plan_v2.md §2).
 *
 * This suite locks the Sprint 6 release contract that every other module builds
 * on top of:
 *   - `MarioState` is exactly { SMALL, SUPER, FIRE_SMALL, FIRE_SUPER }.
 *   - Default character is Mario (`CharacterType::MARIO`); the selection flow
 *     also preserves an explicit Luigi (`CharacterType::LUIGI`) identity.
 *   - FireFlower preserves the body tier: SMALL -> Small Fire, SUPER -> Super Fire.
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

constexpr float TILE_SIZE = 32.0f;

// ===== Runtime contract guards =====

void testDefaultCharacterAndState() {
    std::cout << "[RUNNING] testDefaultCharacterAndState..." << std::endl;

    Mario mario;
    assert(mario.getMarioState() == MarioState::SMALL);
    // Gate 0: a player constructed without a selection defaults to Mario.
    assert(mario.getCharacterType() == CharacterType::MARIO);

    std::cout << "[PASSED] testDefaultCharacterAndState" << std::endl;
}

void testExplicitLuigiIdentity() {
    std::cout << "[RUNNING] testExplicitLuigiIdentity..." << std::endl;

    Mario luigi;
    luigi.setCharacterType(CharacterType::LUIGI);
    assert(luigi.getCharacterType() == CharacterType::LUIGI);

    std::cout << "[PASSED] testExplicitLuigiIdentity" << std::endl;
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

    // S7-TV1-02 dependency evidence: every catalog entry must pass the
    // syntax/tileset loader before PlayState can ever reach Win. This check is
    // deliberately theme-neutral: level 3's catalog metadata is Underwater,
    // while its current map-theme correction remains a TV4-owned external gate.
    assert(LevelCatalog::count() == 4);
    for (const LevelDefinition& definition : LevelCatalog::getAll()) {
        assert(loadReleaseLevel(definition.filePath));
    }
    std::cout << "[INFO] Level 3 Underwater map semantics remain an external TV4 gate; "
                 "syntax/load coverage is intentional here." << std::endl;

    std::cout << "[PASSED] testReleaseLevelsAreLoadable" << std::endl;
}

void testReleaseLevelMarkers() {
    std::cout << "[RUNNING] testReleaseLevelMarkers..." << std::endl;

    for (const LevelDefinition& definition : LevelCatalog::getAll()) {
        TileMap tileMap;
        assert(tileMap.loadFromFile(definition.filePath));
        assert(tileMap.getWidth() > 0);
        assert(tileMap.getHeight() > 0);
        // Validator contract: exactly one Mario spawn and one finish per level.
        assert(tileMap.findTiles('M').size() == 1);
        assert(tileMap.findTiles('F').size() == 1);
    }

    std::cout << "[PASSED] testReleaseLevelMarkers" << std::endl;
}

void testAllLevelFlagMarkers() {
    std::cout << "[RUNNING] testAllLevelFlagMarkers..." << std::endl;

    const std::string levelFiles[] = {
        "levels/level0.txt",
        "levels/level1.txt",
        "levels/level2.txt",
        "levels/level3.txt",
        "levels/level4.txt",
    };

    // Flag markers are syntax/geometry contracts and do not establish a map's
    // semantic theme, so level 3 is checked without selecting a catalog theme.
    for (const std::string& filePath : levelFiles) {
        TileMap tileMap;
        assert(tileMap.loadFromFile(filePath));

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

    for (int levelNumber : {1, 2}) {
        const auto* definition = LevelCatalog::find(levelNumber);

        TileMap tileMap;
        assert(tileMap.loadFromFile(definition->filePath));

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

void testThemeWiringAndLevel4Spawns() {
    std::cout << "[RUNNING] testThemeWiringAndLevel4Spawns..." << std::endl;

    Level castle;
    castle.setTheme(LevelTheme::CASTLE);
    assert(castle.loadFromFile("levels/level4.txt"));

    int enemyCount = 0;

    bool hasPodoboo = false;
    bool hasFirebar = false;
    bool hasBowser = false;
    bool hasBowserAxe = false;

    for (const auto& entity : castle.getEntities()) {
        assert(entity);

        if (entity->isEnemy()) {
            ++enemyCount;
        }

        hasPodoboo = hasPodoboo || entity->isPodoboo();
        hasFirebar = hasFirebar || entity->isFirebar();
        hasBowser = hasBowser || entity->isBowser();
        hasBowserAxe = hasBowserAxe || entity->isBowserAxe();
    }

    // Level 4 is now the Castle finale, so validate its actual Castle mechanis instead of the old Koopa/Piranha/Springboard fixture.
    assert(enemyCount >= 1);
    assert(hasPodoboo);
    assert(hasFirebar);
    assert(hasBowser);
    assert(hasBowserAxe);

    const TileMap& tileMap = castle.getTileMap();

    // Castle finale must contain Bowser's collapsible bridge.
    assert(!tileMap.findTiles('=').empty());

    // W is the theme-aware liquid marker.
    assert(!tileMap.findTiles('W').empty());

    std::cout << "[PASSED] testThemeWiringAndLevel4Spawns" << std::endl;
}

void testFlagAnimationChangesPixelsForValidatedThemes() {
    std::cout << "[RUNNING] testFlagAnimationChangesPixelsForValidatedThemes..." << std::endl;

    struct ThemeCase {
        const char* path;
        LevelTheme theme;
    };

    const ThemeCase cases[] = {
        {"levels/level0.txt", LevelTheme::OVERWORLD},
        {"levels/level1.txt", LevelTheme::OVERWORLD},
        {"levels/level2.txt", LevelTheme::UNDERGROUND},
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

    std::cout << "[PASSED] testFlagAnimationChangesPixelsForValidatedThemes" << std::endl;
}

void testLevel4FlagSequencePublishesOnce() {
    std::cout << "[RUNNING] testLevel4FlagSequencePublishesOnce..." << std::endl;

    Level level;
    level.setTheme(LevelTheme::CASTLE);
    assert(level.loadFromFile("levels/level4.txt"));

    CompletionCounter counter;
    const sf::Vector2i finish = level.getTileMap().findTiles('F').front();
    const sf::Vector2f startPosition =
        TileMap::gridToWorldPosition(finish) + sf::Vector2f(7.0f, 8.0f);
    level.getMario()->setPosition(startPosition);

    // Contact starts the sequence but must not complete the level immediately.
    level.update(0.0f);
    assert(level.getMario()->isFlagpoleSliding());
    // Mario snaps beside the pole column: Y is unchanged, X is aligned against
    // the pole (the previous exact-position assert no longer holds because the
    // fix intentionally repositions Mario flush against the pole).
    assert(level.getMario()->getPosition().y == startPosition.y);
    assert(!level.getMario()->isFlagpoleSlideComplete());
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

    // The Castle pole is traversed at the fixed slide speed. With the fix the
    // sequence no longer uses an estimated timer for the slide, so the full
    // descent + flag drop + walk takes more seconds than the old shortcut.
    for (int step = 0; step < 20 && !level.isLevelCompleted(); ++step) {
        level.update(1.0f);
    }

    assert(level.isLevelCompleted());
    assert(counter.count == 1);
    level.update(1.0f);
    assert(counter.count == 1);

    std::cout << "[PASSED] testLevel4FlagSequencePublishesOnce" << std::endl;
}

void testLevel2FlagSequenceMatchesLevel1() {
    std::cout << "[RUNNING] testLevel2FlagSequenceMatchesLevel1..." << std::endl;

    Level level;
    level.setTheme(LevelTheme::UNDERGROUND);
    assert(level.loadFromFile("levels/level2.txt"));

    const sf::Vector2i finish = level.getTileMap().findTiles('F').front();
    const sf::Vector2f startPosition =
        TileMap::gridToWorldPosition(finish) + sf::Vector2f(7.0f, 8.0f);
    level.getMario()->setPosition(startPosition);
    level.update(0.0f);

    assert(level.getMario()->isFlagpoleSliding());
    assert(!level.isLevelCompleted());

    const float previousFlagDrop = level.getTileMap().getFlagDropDistance();
    level.update(0.25f);
    assert(level.getTileMap().getFlagDropDistance() > previousFlagDrop);

    for (int step = 0; step < 40 && !level.isLevelCompleted(); ++step) {
        level.update(1.0f);
    }

    assert(level.isLevelCompleted());
    assert(level.getTileMap().isFlagFullyDropped());
    const auto castles = level.getTileMap().findTiles('L');
    assert(castles.size() >= 2);
    // Level 2 has a decorative castle at the spawn and the exit castle after
    // the flag. Mario must walk to the latter, as in Level 1.
    assert(level.getMario()->getPosition().x >=
           static_cast<float>(castles.back().x * 32));
    std::cout << "[PASSED] testLevel2FlagSequenceMatchesLevel1" << std::endl;
}

void testFlagWalkReachesCastleWithoutTeleporting() {
    std::cout << "[RUNNING] testFlagWalkReachesCastleWithoutTeleporting..." << std::endl;

    Level level;
    level.setTheme(LevelTheme::OVERWORLD);
    assert(level.loadFromFile("levels/level1.txt"));

    const sf::Vector2i finish = level.getTileMap().findTiles('F').front();
    const sf::Vector2f startPosition =
        TileMap::gridToWorldPosition(finish) + sf::Vector2f(7.0f, 8.0f);
    level.getMario()->setPosition(startPosition);
    level.update(0.0f);

    constexpr float FRAME_DT = 1.0f / 60.0f;
    float largestWalkStep = 0.0f;

    for (int step = 0; step < 600 && !level.isLevelCompleted(); ++step) {
        const bool wasWalking = level.isFlagSequenceActive() &&
                                !level.getMario()->isFlagpoleSliding();
        const float previousX = level.getMario()->getPosition().x;

        level.update(FRAME_DT);

        if (wasWalking) {
            const float currentX = level.getMario()->getPosition().x;
            largestWalkStep = std::max(largestWalkStep, currentX - previousX);
        }
    }

    assert(level.isLevelCompleted());
    // At 60 FPS the automatic walk can advance only a few pixels per update.
    // A larger jump means the old timeout fallback snapped Mario to the door.
    assert(largestWalkStep < 10.0f);

    std::cout << "[PASSED] testFlagWalkReachesCastleWithoutTeleporting" << std::endl;
}

void testFlagSequenceSnapsMarioToPoleSide() {
    std::cout << "[RUNNING] testFlagSequenceSnapsMarioToPoleSide..." << std::endl;

    // Grab from the LEFT of the pole column.
    Level leftLevel;
    leftLevel.setTheme(LevelTheme::CASTLE);
    assert(leftLevel.loadFromFile("levels/level4.txt"));
    const sf::Vector2i finish = leftLevel.getTileMap().findTiles('F').front();
    const sf::Vector2f triggerTopLeft = TileMap::gridToWorldPosition(finish);
    const float poleCenterX = triggerTopLeft.x + TILE_SIZE / 2.0f;

    const sf::Vector2f leftStart(triggerTopLeft.x - 10.0f,
                                 triggerTopLeft.y + 8.0f);
    leftLevel.getMario()->setPosition(leftStart);
    leftLevel.update(0.0f);
    assert(leftLevel.getMario()->isFlagpoleSliding());
    // Left grab: sprite faces right, body offset -14.0f from the pole column.
    assert(leftLevel.getMario()->getFacingDirection() == Direction::RIGHT);
    const float expectedLeftX =
        poleCenterX - leftLevel.getMario()->getSize().x / 2.0f - 14.0f;
    assert(std::abs(leftLevel.getMario()->getPosition().x - expectedLeftX) < 0.01f);

    // Grab from the RIGHT of the pole column.
    Level rightLevel;
    rightLevel.setTheme(LevelTheme::CASTLE);
    assert(rightLevel.loadFromFile("levels/level4.txt"));
    const sf::Vector2f rightStart(triggerTopLeft.x + 20.0f,
                                  triggerTopLeft.y + 8.0f);
    rightLevel.getMario()->setPosition(rightStart);
    rightLevel.update(0.0f);
    assert(rightLevel.getMario()->isFlagpoleSliding());
    // Right grab: sprite flips to face left, body offset +14.0f.
    assert(rightLevel.getMario()->getFacingDirection() == Direction::LEFT);
    const float expectedRightX =
        poleCenterX - rightLevel.getMario()->getSize().x / 2.0f + 14.0f;
    assert(std::abs(rightLevel.getMario()->getPosition().x - expectedRightX) < 0.01f);
    assert(rightLevel.getMario()->getPosition().x +
               rightLevel.getMario()->getSize().x / 2.0f >
           poleCenterX);

    std::cout << "[PASSED] testFlagSequenceSnapsMarioToPoleSide" << std::endl;
}

void testFlagCompletionGatedOnFullFlagDrop() {
    std::cout << "[RUNNING] testFlagCompletionGatedOnFullFlagDrop..." << std::endl;

    Level level;
    level.setTheme(LevelTheme::OVERWORLD);
    assert(level.loadFromFile("levels/level1.txt"));

    CompletionCounter counter;

    const sf::Vector2i finish = level.getTileMap().findTiles('F').front();
    const sf::Vector2f triggerTopLeft = TileMap::gridToWorldPosition(finish);
    assert(level.getTileMap().getFlagMaxDropDistance() > 0.0f);

    // Grab the pole near its base: Mario only slides a short way, so the flag
    // must still descend to its validated maximum before the level completes.
    int bottomPoleRow =finish.y;
    
    for (const sf::Vector2i& pole : level.getTileMap().findTiles('|')) {
        if (pole.x == finish.x && pole.y > bottomPoleRow) {
            bottomPoleRow = pole.y;
        }
    }

    const float lowGrabY = static_cast<float>((bottomPoleRow + 1) * TILE_SIZE) - level.getMario()->getSize().y - 1.0f;
    level.getMario()->setPosition(
        sf::Vector2f(triggerTopLeft.x + 7.0f, lowGrabY));
    level.update(0.0f);
    assert(level.getMario()->isFlagpoleSliding());

    // Large dt spikes must not let the rough time estimate skip the descent.
    // After a few seconds the slide is complete but the flag is not yet fully
    // dropped, so the level must still be running.
    for (int step = 0; step < 3; ++step) {
        level.update(1.0f);
    }
    assert(level.getMario()->isFlagpoleSlideComplete());
    assert(level.getTileMap().getFlagDropDistance() <
           level.getTileMap().getFlagMaxDropDistance());
    assert(!level.isLevelCompleted());
    assert(counter.count == 0);

    // Finish the flag descent, then walk into the castle and publish
    // LEVEL_COMPLETED exactly once.
    for (int step = 0; step < 20 && !level.isLevelCompleted(); ++step) {
        level.update(1.0f);
    }
    assert(level.isLevelCompleted());
    assert(level.getTileMap().isFlagFullyDropped());
    assert(counter.count == 1);
    level.update(1.0f);
    assert(counter.count == 1);

    std::cout << "[PASSED] testFlagCompletionGatedOnFullFlagDrop" << std::endl;
}

} // namespace

int main() {
    testDefaultCharacterAndState();
    testExplicitLuigiIdentity();
    testFireFlowerPreservesBodyTier();
    testFireFlowerGrantsFireFromSuper();
    testMushroomPromotesAndNeverDowngrades();
    testReleaseLevelsAreLoadable();
    testReleaseLevelMarkers();
    testAllLevelFlagMarkers();
    testRandomQuestionBlocksAndItemRoutes();
    testFireBallActiveLimitOfTwo();
    testThemeWiringAndLevel4Spawns();
    testFlagAnimationChangesPixelsForValidatedThemes();
    testLevel4FlagSequencePublishesOnce();
    testLevel2FlagSequenceMatchesLevel1();
    testFlagWalkReachesCastleWithoutTeleporting();
    testFlagSequenceSnapsMarioToPoleSide();
    testFlagCompletionGatedOnFullFlagDrop();

    std::cout << "All Gate0 contract tests passed successfully!" << std::endl;
    return 0;
}
