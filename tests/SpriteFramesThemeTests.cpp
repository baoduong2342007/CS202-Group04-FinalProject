/**
 * @file SpriteFramesThemeTests.cpp
 * @brief Compile/runtime regression checks for the split sprite-frame catalogs.
 */

#include <cassert>
#include <iostream>
#include <vector>

#include "core/SpriteFrames_shared.h"
#include "core/SpriteFrames_ovw.h"
#include "core/SpriteFrames_udg.h"
#include "core/SpriteFrames_castle.h"
#include "core/SpriteFrames_udw.h"
#include "core/SpriteFrames.h"
#include "level/TileFrames.h"

namespace {

namespace OvwKoopa = SpriteFrames::ovw::Enemies::Koopa;
namespace UdgKoopa = SpriteFrames::udg::Enemies::Koopa;
namespace CastleKoopa = SpriteFrames::castle::Enemies::Koopa;
namespace UdwKoopa = SpriteFrames::udw::Enemies::Koopa;

namespace OvwGoomba = SpriteFrames::ovw::Enemies::Goomba;
namespace UdgGoomba = SpriteFrames::udg::Enemies::Goomba;
namespace CastleGoomba = SpriteFrames::castle::Enemies::Goomba;
namespace UdwGoomba = SpriteFrames::udw::Enemies::Goomba;

bool hasPosition(const sf::IntRect& frame, int x, int y) {
    return frame.position.x == x && frame.position.y == y;
}

bool sameGeometry(const sf::IntRect& left, const sf::IntRect& right) {
    return left.size.x == right.size.x && left.size.y == right.size.y;
}

void testNamespacesAndFacade() {
    // Referencing one normalized frame from each theme verifies all namespaces
    // and headers are independently usable.
    assert(SpriteFrames::ovw::Enemies::Goomba::WALK1.size.x == 16);
    assert(SpriteFrames::udg::Enemies::Goomba::WALK1.size.x == 16);
    assert(SpriteFrames::castle::Enemies::Goomba::WALK1.size.x == 16);
    assert(SpriteFrames::udw::Enemies::Goomba::WALK1.size.x == 16);

    // Common frames are explicitly shared.
    assert(SpriteFrames::shared::SmallMario::IDLE.size.x == 16);
    assert(SpriteFrames::shared::Items::coinFrames().size() == 4);

    // The old facade remains source-compatible.
    assert(SpriteFrames::IS_FACADE_ACTIVE);
    assert(SpriteFrames::SmallMario::IDLE.size.x == 16);
    assert(SpriteFrames::Enemies::GreenEnemiesBlock::UG_KOOPA_WALK1.size.x == 16);
    assert(SpriteFrames::ovw::Enemies::Koopa::WALK1.size.x == 16);
    assert(SpriteFrames::Blocks::UG_EMPTY.size.x == 16);
    assert(SpriteFrames::Items::coinFrames().size() == 4);
}

void testUndergroundKoopaCoordinates() {
    assert(hasPosition(UdgKoopa::WALK1, 146, 112));
    assert(hasPosition(UdgKoopa::WALK2, 164, 112));
    assert(hasPosition(UdgKoopa::SHELL, 218, 120));

    // Castle intentionally aliases the same verified palette row.
    assert(hasPosition(CastleKoopa::WALK1, 146, 112));
    assert(hasPosition(CastleKoopa::WALK2, 164, 112));
    assert(hasPosition(CastleKoopa::SHELL, 218, 120));
}

void testKoopaAliasesUseVerifiedGreenEnemyFrames() {
    assert(hasPosition(OvwKoopa::WALK1, 0, 112));
    assert(hasPosition(OvwKoopa::WALK2, 18, 112));
    assert(hasPosition(OvwKoopa::SHELL, 72, 120));

    assert(hasPosition(UdgKoopa::WALK1, 146, 112));
    assert(hasPosition(UdgKoopa::WALK2, 164, 112));
    assert(hasPosition(UdgKoopa::SHELL, 218, 120));

    // The atlas-block compatibility view must resolve to the same frames.
    assert(SpriteFrames::ovw::Enemies::GreenEnemiesBlock::WALK1.position.x ==
           OvwKoopa::WALK1.position.x);
    assert(SpriteFrames::ovw::Enemies::GreenEnemiesBlock::WALK2.position.y ==
           OvwKoopa::WALK2.position.y);
}

void testThemeFrameGeometry() {
    for (const sf::IntRect* frame : {
             &OvwKoopa::WALK1,
             &UdgKoopa::WALK1,
             &CastleKoopa::WALK1,
             &UdwKoopa::WALK1}) {
        assert(frame->size.x == 16 && frame->size.y == 24);
    }

    assert(sameGeometry(OvwKoopa::WALK1, UdgKoopa::WALK1));
    assert(sameGeometry(OvwKoopa::WALK2, CastleKoopa::WALK2));
    assert(sameGeometry(UdgKoopa::SHELL, UdwKoopa::SHELL));

    assert(sameGeometry(OvwGoomba::WALK1, UdgGoomba::WALK1));
    assert(sameGeometry(UdgGoomba::WALK2, CastleGoomba::WALK2));
    assert(sameGeometry(CastleGoomba::STOMPED, UdwGoomba::STOMPED));

    assert(sameGeometry(SpriteFrames::ovw::Blocks::QUESTION1,
                        SpriteFrames::udg::Blocks::QUESTION1));
    assert(sameGeometry(SpriteFrames::udg::Blocks::QUESTION2,
                        SpriteFrames::castle::Blocks::QUESTION2));
    assert(sameGeometry(SpriteFrames::castle::Blocks::QUESTION3,
                        SpriteFrames::udw::Blocks::QUESTION3));
}

void testCanonicalEnemyCoordinates() {
    namespace Enemies = SpriteFrames::legacy::Enemies;

    // Ground-enemy rows: the UG/Castle/Underwater entries must not drift into
    // the adjacent Bullet Bill or Blooper cells.
    assert(hasPosition(Enemies::Blooper::UG_SWIM_OPEN, 74, 52));
    assert(Enemies::Blooper::UG_SWIM_OPEN.size.x == 16 &&
           Enemies::Blooper::UG_SWIM_OPEN.size.y == 24);
    assert(hasPosition(Enemies::Blooper::CASTLE_SWIM_OPEN, 148, 52));
    assert(hasPosition(Enemies::Blooper::UW_SWIM_OPEN, 222, 52));
    assert(hasPosition(Enemies::BulletBill::UG_BULLET, 128, 52));
    assert(hasPosition(Enemies::BulletBill::CASTLE_BULLET, 202, 52));

    // The Underwater Koopa pair is #61/#62; #63/#64 are Paratroopa.
    assert(hasPosition(Enemies::Koopa::UW_WALK1, 292, 112));
    assert(hasPosition(Enemies::Koopa::UW_WALK2, 310, 112));
    assert(hasPosition(Enemies::GreenEnemiesBlock::UW_PARATROOPA_FLY1,
                       328, 112));
    assert(hasPosition(Enemies::GreenEnemiesBlock::UW_PARATROOPA_FLY2,
                       346, 112));

    // Piranha Plant uses open/closed order in every palette group.
    assert(hasPosition(Enemies::PiranhaPlant::OPEN, 0, 138));
    assert(hasPosition(Enemies::PiranhaPlant::CLOSED, 18, 138));
    assert(hasPosition(Enemies::PiranhaPlant::UG_CASTLE_OPEN, 146, 138));
    assert(hasPosition(Enemies::PiranhaPlant::UG_CASTLE_CLOSED, 164, 138));
    assert(hasPosition(Enemies::PiranhaPlant::UW_OPEN, 292, 138));
    assert(hasPosition(Enemies::PiranhaPlant::UW_CLOSED, 310, 138));

    // The large Castle rows are Bowser hammer poses; only #139/#140 are the
    // small Hammer Bro special frames.
    assert(hasPosition(Enemies::Bowser::THROW_RIGHT, 68, 242));
    assert(Enemies::Bowser::THROW_RIGHT.size.x == 32 &&
           Enemies::Bowser::THROW_RIGHT.size.y == 26);
    assert(hasPosition(Enemies::Bowser::UG_THROW_RIGHT, 214, 242));
    assert(hasPosition(Enemies::HammerBro::BOWSER_HAMMER_LEFT1, 34, 276));
    assert(hasPosition(Enemies::HammerBro::THROW1, 144, 276));
    assert(hasPosition(Enemies::HammerBro::BOWSER_HAMMER_RIGHT1, 180, 276));

    // Red row order is Koopa, Paratroopa, shell; Spiny walk is #153 -> #154.
    assert(hasPosition(Enemies::RedKoopa::WALK1, 0, 318));
    assert(hasPosition(Enemies::RedKoopa::WALK2, 18, 318));
    assert(hasPosition(Enemies::RedKoopa::PARATROOPA1, 36, 318));
    assert(hasPosition(Enemies::RedKoopa::PARATROOPA2, 54, 318));
    assert(hasPosition(Enemies::RedSpiny::WALK1, 72, 352));
    assert(hasPosition(Enemies::RedSpiny::WALK2, 90, 352));
}

void testCanonicalTileCoordinates() {
    assert(hasPosition(TileFrames::GROUND, 0, 16));
    assert(hasPosition(TileFrames::BRICK, 17, 16));
    assert(hasPosition(TileFrames::STONE, 0, 33));
    assert(hasPosition(TileFrames::BRICK_UNDERGROUND, 164, 16));
    assert(hasPosition(TileFrames::BRICK_VARIANT_UNDERGROUND, 181, 16));
    assert(hasPosition(TileFrames::STONE_UNDERGROUND, 147, 33));
    assert(hasPosition(TileFrames::HARD_BLOCK_UNDERGROUND, 445, 78));

    // Question/used blocks are in Palette 3, not beside the terrain quartet.
    assert(hasPosition(TileFrames::QUESTION, 298, 78));
    assert(hasPosition(TileFrames::QUESTION_UNDERGROUND, 394, 78));
    assert(hasPosition(TileFrames::QUESTION_CASTLE, 490, 78));
    assert(hasPosition(TileFrames::USED_BLOCK, 349, 78));
    assert(hasPosition(TileFrames::USED_BLOCK_UNDERGROUND, 445, 78));
    assert(hasPosition(TileFrames::USED_BLOCK_CASTLE, 541, 78));

    // A normal map coin is a 16x16 tileset animation, separate from the
    // items_objects coin used for a QuestionBlock popup.
    assert(hasPosition(TileFrames::COIN_OVERWORLD, 298, 95));
    assert(hasPosition(TileFrames::COIN_OVERWORLD_SIDE, 315, 95));
    assert(hasPosition(TileFrames::COIN_OVERWORLD_THIN, 332, 95));

    // Castle palette pipe/pole frames mirror the Overworld geometry.
    assert(hasPosition(TileFrames::PIPE_TOP_LEFT_CASTLE, 119, 280));
    assert(hasPosition(TileFrames::PIPE_BODY_RIGHT_CASTLE, 136, 297));
    assert(hasPosition(TileFrames::FINISH_TOP_CASTLE, 136, 314));
    assert(hasPosition(TileFrames::FINISH_POLE_CASTLE, 136, 331));

    // TV4 composite scenery stored directly in the runtime atlas.
    assert(hasPosition(TileFrames::FINISH_TOP, 136, 230));

    assert(hasPosition(TileFrames::CASTLE, 328, 196));
    assert(TileFrames::CASTLE.size.x == 80);
    assert(TileFrames::CASTLE.size.y == 80);

    assert(hasPosition(TileFrames::HORIZONTAL_PIPE, 328, 298));
    assert(TileFrames::HORIZONTAL_PIPE.size.x == 48);
    assert(TileFrames::HORIZONTAL_PIPE.size.y == 32);
}

void testCanonicalFlagpoleCoordinates() {
    assert(hasPosition(SpriteFrames::ovw::Items::FLAGPOLE_FLAG, 92, 90));
    assert(SpriteFrames::ovw::Items::FLAGPOLE_FLAG.size.x == 16 &&
           SpriteFrames::ovw::Items::FLAGPOLE_FLAG.size.y == 16);

    assert(hasPosition(SpriteFrames::udg::Items::FLAGPOLE_FLAG, 92, 198));
    assert(SpriteFrames::udg::Items::FLAGPOLE_FLAG.size.x == 16 &&
           SpriteFrames::udg::Items::FLAGPOLE_FLAG.size.y == 16);

    assert(hasPosition(SpriteFrames::castle::Items::FLAGPOLE_FLAG, 92, 306));
    assert(SpriteFrames::castle::Items::FLAGPOLE_FLAG.size.x == 16 &&
           SpriteFrames::castle::Items::FLAGPOLE_FLAG.size.y == 16);

    assert(hasPosition(SpriteFrames::udw::Items::FLAGPOLE_FLAG, 92, 414));
    assert(SpriteFrames::udw::Items::FLAGPOLE_FLAG.size.x == 16 &&
           SpriteFrames::udw::Items::FLAGPOLE_FLAG.size.y == 15);

    // T is the round pole cap and uses the matching themed tileset palette.
    assert(hasPosition(TileFrames::FINISH_TOP, 136, 230));
    assert(hasPosition(TileFrames::FINISH_TOP_UNDERGROUND, 300, 230));
    assert(hasPosition(TileFrames::FINISH_TOP_CASTLE, 136, 314));
    assert(hasPosition(TileFrames::FINISH_TOP_UNDERWATER, 300, 314));

    // The old names remain valid aliases after correcting the adjacent atlas
    // entries for castle flag and vine frames.
    assert(hasPosition(SpriteFrames::legacy::Items::CASTLE_FLAG, 110, 90));
    assert(hasPosition(SpriteFrames::legacy::Items::VINE_TOP, 130, 90));
    assert(hasPosition(SpriteFrames::legacy::Items::VINE_STEM_1, 148, 90));
    assert(hasPosition(SpriteFrames::legacy::Items::VINE_STEM_2, 148, 90));
}

void testMushroomCoordinates() {
    assert(hasPosition(SpriteFrames::ovw::Items::SUPER_MUSHROOM, 0, 8));
    assert(hasPosition(SpriteFrames::ovw::Items::ONE_UP_MUSHROOM, 0, 26));
    assert(hasPosition(SpriteFrames::udg::Items::SUPER_MUSHROOM, 0, 116));
    assert(hasPosition(SpriteFrames::udg::Items::ONE_UP_MUSHROOM, 0, 134));
    assert(hasPosition(SpriteFrames::castle::Items::SUPER_MUSHROOM, 0, 224));
    assert(hasPosition(SpriteFrames::castle::Items::ONE_UP_MUSHROOM, 0, 242));
    assert(hasPosition(SpriteFrames::udw::Items::SUPER_MUSHROOM, 0, 332));
    assert(hasPosition(SpriteFrames::udw::Items::ONE_UP_MUSHROOM, 0, 350));
}

void testScoreCoordinates() {
    using namespace SpriteFrames::shared::Items;
    assert(hasPosition(SCORE_100, 234, 26));
    assert(hasPosition(SCORE_1000, 252, 26));
    assert(hasPosition(SCORE_200, 234, 36));
    assert(hasPosition(SCORE_2000, 252, 36));
    assert(hasPosition(SCORE_400, 234, 46));
    assert(hasPosition(SCORE_4000, 252, 46));
    assert(hasPosition(SCORE_500, 234, 56));
    assert(hasPosition(SCORE_5000, 252, 56));
    assert(hasPosition(SCORE_800, 234, 66));
    assert(hasPosition(SCORE_8000, 252, 66));
    assert(hasPosition(SCORE_1UP, 252, 76));
}

void testBackgroundConstants() {
    assert(SpriteFrames::legacy::Backgrounds::OVERWORLD_PATH == "assets/textures/backgrounds/bg_overworld.png");
    assert(SpriteFrames::legacy::Backgrounds::UNDERGROUND_PATH == "assets/textures/backgrounds/bg_underground.png");
    assert(SpriteFrames::legacy::Backgrounds::UNDERWATER_PATH == "assets/textures/backgrounds/bg_underwater.png");
    assert(SpriteFrames::legacy::Backgrounds::CASTLE_PATH == "assets/textures/backgrounds/bg_castle.png");
    assert(SpriteFrames::legacy::Backgrounds::THEME_BG.size.x == 640 &&
           SpriteFrames::legacy::Backgrounds::THEME_BG.size.y == 360);

    assert(SpriteFrames::ovw::Backgrounds::OVERWORLD_PATH == "assets/textures/backgrounds/bg_overworld.png");
    assert(SpriteFrames::udg::Backgrounds::UNDERGROUND_PATH == "assets/textures/backgrounds/bg_underground.png");
    assert(SpriteFrames::udw::Backgrounds::UNDERWATER_PATH == "assets/textures/backgrounds/bg_underwater.png");
    assert(SpriteFrames::castle::Backgrounds::CASTLE_PATH == "assets/textures/backgrounds/bg_castle.png");
    assert(SpriteFrames::ovw::Backgrounds::THEME_BG.size.x == 640);
    assert(SpriteFrames::udg::Backgrounds::THEME_BG.size.x == 640);
    assert(SpriteFrames::udw::Backgrounds::THEME_BG.size.x == 640);
    assert(SpriteFrames::castle::Backgrounds::THEME_BG.size.x == 640);
}

} // namespace

int main() {
    testNamespacesAndFacade();
    testUndergroundKoopaCoordinates();
    testKoopaAliasesUseVerifiedGreenEnemyFrames();
    testThemeFrameGeometry();
    testCanonicalEnemyCoordinates();
    testCanonicalTileCoordinates();
    testCanonicalFlagpoleCoordinates();
    testMushroomCoordinates();
    testScoreCoordinates();
    testBackgroundConstants();

    std::cout << "SpriteFramesThemeTests passed." << std::endl;
    return 0;
}
