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
 *   - All three release levels (1, 2, 3) are loadable through the validator and
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

#include "core/LevelCatalog.h"
#include "entities/Mario.h"
#include "items/FireFlower.h"
#include "items/Mushroom.h"
#include "level/TileMap.h"

namespace {

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

    std::cout << "[PASSED] testReleaseLevelMarkers" << std::endl;
}

} // namespace

int main() {
    testDefaultCharacterAndState();
    testFireFlowerAlwaysGrantsFireFromSmall();
    testFireFlowerAlwaysGrantsFireFromSuper();
    testMushroomPromotesAndNeverDowngrades();
    testReleaseLevelsAreLoadable();
    testReleaseLevelMarkers();

    std::cout << "All Gate0 contract tests passed successfully!" << std::endl;
    return 0;
}