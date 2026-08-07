/**
 * @file PhysicsEngine.cpp
 * @author TV3
 * @brief Implementation of the PhysicsEngine singleton
 */

#include "physics/PhysicsEngine.h"

float PhysicsEngine::s_timeAccumulator = 0.0f;

bool PhysicsEngine::update(b2World& world, float dt) {
    s_timeAccumulator += dt;
    bool stepped = false;
    while (s_timeAccumulator >= TIME_STEP) {
        world.Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
        s_timeAccumulator -= TIME_STEP;
        stepped = true;
    }
    return stepped;
}
