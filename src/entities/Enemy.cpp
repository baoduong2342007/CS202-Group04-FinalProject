/**
 * @file Enemy.cpp
 * @author TV4 (Vy)
 * @brief Implementation of the abstract Enemy base class
 */

#include "entities/Enemy.h"
#include "level/TileMap.h"

#include <cmath>
#include <algorithm>

Enemy::Enemy(const sf::Vector2f& position, const sf::Vector2f& size, int health)
: Character(position, size, health){}

void Enemy::updatePlayerPosition(const sf::Vector2f& playerPos) {
    m_playerPos = playerPos;
    m_hasPlayerPos = true;
}

bool Enemy::isPlayerNearby(float horizontalRadius, float verticalRadius) const {
    if (!m_hasPlayerPos) {
        return false;
    }
    const float dx = std::abs(m_playerPos.x - (m_position.x + m_size.x / 2.f));
    const float dy = std::abs(m_playerPos.y - (m_position.y + m_size.y / 2.f));
    return (dx <= horizontalRadius && dy <= verticalRadius);
}

bool Enemy::isPlayerInFront() const {
    if (!m_hasPlayerPos) {
        return false;
    }
    const float enemyMidX = m_position.x + m_size.x / 2.f;
    if (getFacingDirection() == Direction::LEFT) {
        return m_playerPos.x < enemyMidX;
    } else {
        return m_playerPos.x > enemyMidX;
    }
}

bool Enemy::isPlayerBelow() const {
    if (!m_hasPlayerPos) {
        return false;
    }
    return (m_playerPos.y + 16.f >= m_position.y);
}

bool Enemy::isSafeDropAhead(const TileMap* tileMap, int maxDropTiles) const {
    if (!tileMap) {
        return false;
    }
    constexpr float TILE_SIZE = 32.f;
    constexpr float EDGE_PROBE_OFFSET = 2.f;

    const float frontX = (getFacingDirection() == Direction::LEFT)
        ? (m_position.x - EDGE_PROBE_OFFSET)
        : (m_position.x + m_size.x + EDGE_PROBE_OFFSET);
    const int frontColumn = static_cast<int>(std::floor(frontX / TILE_SIZE));

    const float footY = m_position.y + m_size.y + EDGE_PROBE_OFFSET;
    const int startRow = static_cast<int>(std::floor(footY / TILE_SIZE));

    for (int r = startRow; r < startRow + maxDropTiles; ++r) {
        if (tileMap->isEnemySupport(frontColumn, r)) {
            return true;
        }
    }
    return false;
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

void Enemy::allowNextDefeat() {
    m_defeatCommitted = false;
}
