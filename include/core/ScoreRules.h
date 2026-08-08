/**
 * @file ScoreRules.h
 * @author TV5 (Truyền)
 * @brief Central score catalog shared by item and gameplay producers.
 */

#pragma once

class Mario;

enum class ScoreEvent {
    COIN_COLLECTED,
    POWER_UP_COLLECTED,
    ENEMY_STOMPED,
    SHELL_DEFEATED,
    FIREBALL_DEFEATED,
    STAR_DEFEATED
};

namespace ScoreRules {

/// Return the single authoritative score value for a gameplay event.
constexpr int pointsFor(ScoreEvent event) {
    switch (event) {
        case ScoreEvent::COIN_COLLECTED:
            return 100;
        case ScoreEvent::POWER_UP_COLLECTED:
            return 1000;
        case ScoreEvent::ENEMY_STOMPED:
            return 100;
        case ScoreEvent::SHELL_DEFEATED:
        case ScoreEvent::FIREBALL_DEFEATED:
        case ScoreEvent::STAR_DEFEATED:
            return 200;
    }

    return 0;
}

/// Apply a catalogued score award to the authoritative Mario score.
void award(Mario& mario, ScoreEvent event);

} // namespace ScoreRules
