/**
 * @file PhysicsEngine.cpp
 * @author TV3
 * @brief Implementation of the PhysicsEngine singleton
 */

#include "physics/PhysicsEngine.h"
#include "physics/ContactListener.h"

PhysicsEngine& PhysicsEngine::getInstance() {
    static PhysicsEngine instance;
    return instance;
}

void PhysicsEngine::init(const sf::Vector2f& gravity) {
    b2Vec2 b2Gravity(pixelsToMeters(gravity.x), pixelsToMeters(gravity.y));
    m_world = std::make_unique<b2World>(b2Gravity);
    
    // Initialize contact listener
    m_contactListener = std::make_unique<ContactListener>();
    m_world->SetContactListener(m_contactListener.get());
    
    m_timeAccumulator = 0.0f;
}

void PhysicsEngine::update(float dt) {
    if (!m_world) return;

    m_timeAccumulator += dt;
    while (m_timeAccumulator >= TIME_STEP) {
        m_world->Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
        m_timeAccumulator -= TIME_STEP;
    }
}
