/**
 * @file PhysicsEngine.cpp
 * @author TV3
 * @brief Implementation of the PhysicsEngine singleton
 */

#include "physics/PhysicsEngine.h"

bool PhysicsEngine::update(b2World& world, float dt, float& timeAccumulator) {
    timeAccumulator += dt;
    bool stepped = false;
    int steps = 0;
    while (timeAccumulator >= TIME_STEP && steps < MAX_SUBSTEPS) {
        world.Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
        timeAccumulator -= TIME_STEP;
        stepped = true;
        steps++;
    }
    if (steps >= MAX_SUBSTEPS) {
        timeAccumulator = 0.0f; // Drop excess accumulated time on frame spike
    }
    return stepped;
}
