/**
 * @file PhysicsEngine.cpp
 * @author TV3
 * @brief Implementation of the PhysicsEngine singleton
 */

#include "physics/PhysicsEngine.h"

void PhysicsEngine::update(b2World& world, float dt) {
    float stepTime = (dt > 0.05f) ? 0.05f : dt;
    world.Step(stepTime, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
}

