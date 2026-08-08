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
    int substepCount = 0;
    const int MAX_SUBSTEPS = 8;
    
    while (s_timeAccumulator >= TIME_STEP && substepCount < MAX_SUBSTEPS) {
        world.Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
        s_timeAccumulator -= TIME_STEP;
        stepped = true;
        substepCount++;
    }
    
    if (substepCount == MAX_SUBSTEPS) {
        s_timeAccumulator = 0.0f; // Drop time to prevent lag buildup
    }
    
    return stepped;
}
