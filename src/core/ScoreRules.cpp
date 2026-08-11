/**
 * @file ScoreRules.cpp
 * @author TV5 (Truyền)
 * @brief Implementation of the shared score catalog.
 */

#include "core/ScoreRules.h"

#include "entities/Mario.h"

namespace ScoreRules {

void award(Mario& mario, ScoreEvent event) {
    mario.addScore(pointsFor(event));
}

} // namespace ScoreRules
