/**
 * @file Enemy.cpp
 * @author TV4 (Vy)
 * @brief Implementation of the abstract Enemy base class
 */

#include "entities/Enemy.h"

#include <cmath>
#include <algorithm>

Enemy::Enemy(const sf::Vector2f& position, const sf::Vector2f& size, int health)
: Character(position, size, health){}

void Enemy::updatePlayerPosition(const sf::Vector2f& playerPos) {
    m_playerPos = playerPos;
    m_hasPlayerPos = true;
}

bool Enemy::isEscapingNarrowRange() const {
    return m_isEscapingNarrowRange;
}

void Enemy::notifyTurnaround() {
    if (!m_hasPlayerPos) {
        m_lastTurnX = m_position.x;
        return;
    }

    if (m_lastTurnX <= -99990.f) {
        m_lastTurnX = m_position.x;
        return;
    }

    const float distance = std::abs(m_position.x - m_lastTurnX);
    // 2.5 tiles * 32 = 80px (covers 1 to 2 tile narrow platforms/corridors)
    constexpr float NARROW_RANGE_MAX_DISTANCE = 80.f;

    if (distance >= 0.1f && distance <= NARROW_RANGE_MAX_DISTANCE) {
        m_narrowTurnCount++;
        if (m_narrowTurnCount >= 1) {
            // The enemy is trapped oscillating in a 1-2 tile area.
            // Steer towards the player's horizontal side to break out.
            m_isEscapingNarrowRange = true;
            const Direction escapeDir =
                (m_playerPos.x < m_position.x) ? Direction::LEFT : Direction::RIGHT;
            setFacingDirection(escapeDir);
            m_trappedMinX = std::min(m_lastTurnX, m_position.x);
            m_trappedMaxX = std::max(m_lastTurnX, m_position.x);
            m_trappedY = m_position.y;
        }
    } else if (distance > NARROW_RANGE_MAX_DISTANCE) {
        m_narrowTurnCount = 0;
        m_isEscapingNarrowRange = false;
    }

    m_lastTurnX = m_position.x;
}

void Enemy::updateNarrowEscapeStatus() {
    if (!m_isEscapingNarrowRange) {
        return;
    }

    // Terminate escape when the enemy has fallen down or moved at least 1 full tile (34px) beyond the trapped edge
    const bool escapedX = (m_position.x < m_trappedMinX - 34.f) ||
                          (m_position.x > m_trappedMaxX + 34.f);
    const bool escapedY = (m_position.y > m_trappedY + 16.f);

    if (escapedX || escapedY) {
        m_isEscapingNarrowRange = false;
        m_narrowTurnCount = 0;
        m_lastTurnX = -99999.f;
    }
}

void Enemy::onSideCollision(Entity* other) {
    (void)other;
}

void Enemy::onFireHit() {
    takeDamage(100);
    markForRemoval();
}

void Enemy::onStarHit() {
    onFireHit();
}

void Enemy::activate() {
    m_activated = true;
}

bool Enemy::isActivated() const {
    return m_activated;
}

bool Enemy::tryCommitDefeat() {
    if (m_defeatCommitted || isDead() || shouldRemove() ||
        isPendingDestroy() || !isActive()) {
        return false;
    }

    m_defeatCommitted = true;
    return true;
}

bool Enemy::tryCommitStomp() {
    if (m_stompCommitted || isDead() || shouldRemove() ||
        isPendingDestroy() || !isActive()) {
        return false;
    }

    m_stompCommitted = true;
    return true;
}

void Enemy::allowNextStomp() {
    m_stompCommitted = false;
}
