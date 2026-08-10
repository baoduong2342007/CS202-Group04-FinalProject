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

} // namespace

int main() {
    testNamespacesAndFacade();
    testUndergroundKoopaCoordinates();
    testKoopaAliasesUseVerifiedGreenEnemyFrames();
    testThemeFrameGeometry();

    std::cout << "SpriteFramesThemeTests passed." << std::endl;
    return 0;
}
