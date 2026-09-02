/**
 * @file CheepCheepConfig.h
 * @author TV4 (Vy) & CS202 Team
 * @brief External Cheep Cheep spawn and generator registry (levels/cheep_cheep.txt)
 * @note Enables spawning Cheep Cheeps without modifying the level map files.
 */

#pragma once

#include <string>
#include <vector>

#include <SFML/System/Vector2.hpp>
#include "entities/Character.h"
#include "entities/CheepCheep.h"

/// Static / swimming spawn point declared in external config
struct CheepCheepSpawnConfig {
    sf::Vector2i gridPosition;
    CheepCheepBehavior behavior = CheepCheepBehavior::SWIMMING;
    CheepCheepColor color = CheepCheepColor::GREEN;
    Direction direction = Direction::LEFT;
};

/// Route-based patrol/oscillation declared in external config (start point to end point)
struct CheepCheepRouteConfig {
    sf::Vector2i start;
    sf::Vector2i end;
    float speedPixelsPerSecond = 40.0f;
    CheepCheepColor color = CheepCheepColor::GREEN;
};

/// Dynamic leap generator zone declared in external config (e.g. overworld bridges)
struct CheepCheepGeneratorConfig {
    int startColumn = 0;
    int endColumn = 0;
    float intervalSeconds = 1.5f;
    float minVx = -80.f;
    float maxVx = 80.f;
    float minVy = -700.f;
    float maxVy = -550.f;
    CheepCheepColor color = CheepCheepColor::RED;
};

#include <string_view>

namespace CheepCheepConfig {

/// Static spawns for the given level file (e.g. "levels/level2.txt" or "level2.txt").
const std::vector<CheepCheepSpawnConfig>& spawnsFor(std::string_view levelFilePath);

/// Route-based Cheep Cheeps for the given level file (start -> end).
const std::vector<CheepCheepRouteConfig>& routesFor(std::string_view levelFilePath);

/// Active generator zones for the given level file.
const std::vector<CheepCheepGeneratorConfig>& generatorsFor(std::string_view levelFilePath);

} // namespace CheepCheepConfig
