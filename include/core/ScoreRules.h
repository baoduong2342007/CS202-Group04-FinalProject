/**
 * @file ScoreRules.h
 * @author TV5 (Truyền)
 * @brief Central score catalog shared by item and gameplay producers.
 */

#pragma once

class Mario;

/// Gameplay reason used by every enemy-defeat score producer.
///
/// PIT is intentionally represented for the shared release contract, but a
/// pit death awards no enemy score.
enum class DefeatCause {
    STOMP,
    SHELL,
    FIREBALL,
    STAR,
    BLOCK_BUMP,
    PIT
};

enum class ScoreEvent {
    COIN_COLLECTED,
    POWER_UP_COLLECTED,
    ENEMY_STOMPED,
    SHELL_DEFEATED,
    FIREBALL_DEFEATED,
    STAR_DEFEATED,
    BLOCK_BUMP_DEFEATED
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
        case ScoreEvent::BLOCK_BUMP_DEFEATED:
            return 100;
    }

    return 0;
}

/// Return the score value for a shared defeat cause.
constexpr int pointsFor(DefeatCause cause) {
    switch (cause) {
        case DefeatCause::STOMP:
            return pointsFor(ScoreEvent::ENEMY_STOMPED);
        case DefeatCause::SHELL:
            return pointsFor(ScoreEvent::SHELL_DEFEATED);
        case DefeatCause::FIREBALL:
            return pointsFor(ScoreEvent::FIREBALL_DEFEATED);
        case DefeatCause::STAR:
            return pointsFor(ScoreEvent::STAR_DEFEATED);
        case DefeatCause::BLOCK_BUMP:
            return pointsFor(ScoreEvent::BLOCK_BUMP_DEFEATED);
        case DefeatCause::PIT:
            return 0;
    }

    return 0;
}

/// Apply a catalogued score award to the authoritative Mario score.
void award(Mario& mario, ScoreEvent event);

} // namespace ScoreRules
