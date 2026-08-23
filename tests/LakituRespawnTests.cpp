/**
 * @file LakituRespawnTests.cpp
 * @author TV1
 * @brief Regression: a respawned Lakitu must be defeatable again.
 *
 * Lakitu legitimately comes back from a terminal defeat (SMB1-style corpse
 * fall followed by a revive). The one-shot defeat transaction therefore has
 * to be re-armed during respawn. Before the fix the latch stayed committed,
 * so every later fireball, star, or shell hit bounced off an immortal
 * Lakitu and only stomps could finish it.
 */

#include <cassert>

#include <box2d/box2d.h>

#include "entities/Lakitu.h"

namespace {

void testRespawnedLakituIsDefeatableAgain() {
    b2World world(b2Vec2(0.f, 25.f)); // canonical overworld gravity
    Lakitu lakitu(sf::Vector2f{200.f, 96.f}, &world, LevelTheme::OVERWORLD);
    lakitu.activate();

    // The first non-stomp defeat claims the shared transaction, then the
    // fireball response flips the corpse into its falling presentation.
    assert(lakitu.tryCommitDefeat());
    assert(!lakitu.tryCommitDefeat()); // latched while knocked down
    lakitu.onFireHit();

    // Step physics plus the entity until the corpse sinks past RESPAWN_Y
    // and Lakitu revives behind the player.
    bool respawned = false;
    for (int i = 0; i < 900 && !respawned; ++i) {
        world.Step(1.f / 60.f, 8, 3);
        lakitu.update(1.f / 60.f);
        if (!lakitu.isDying()) {
            respawned = true;
        }
    }
    assert(respawned);

    // THE regression: the revived Lakitu must accept a fresh defeat claim so
    // fireballs, stars, and shells can knock it down again.
    assert(lakitu.tryCommitDefeat());
}

} // namespace

int main() {
    testRespawnedLakituIsDefeatableAgain();
    return 0;
}
