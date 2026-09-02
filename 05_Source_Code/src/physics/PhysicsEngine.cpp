/**
 * @file PhysicsEngine.cpp
 * @author TV3
 * @brief Implementation of the PhysicsEngine singleton
 */

#include "physics/PhysicsEngine.h"

bool PhysicsEngine::update(b2World& world, float dt, float& timeAccumulator) {
    timeAccumulator += dt;
    bool stepped = false;
    int substepCount = 0;
    
    while (timeAccumulator >= TIME_STEP && substepCount < MAX_SUBSTEPS) {
        world.Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
        timeAccumulator -= TIME_STEP;
        stepped = true;
        substepCount++;
    }
    
    if (substepCount == MAX_SUBSTEPS) {
        timeAccumulator = 0.0f; // Drop time to prevent lag buildup
    }
    
    return stepped;
}
