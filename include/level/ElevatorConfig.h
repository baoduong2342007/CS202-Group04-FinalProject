/**
 * @file ElevatorConfig.h
 * @author TV1 (Duong)
 * @brief External elevator-route registry read from levels/elevators.txt
 * @note Lets levels get moving platforms WITHOUT modifying the level map
 *       files. Routes are declared in a separate config file keyed by level.
 */

#pragma once

#include <string>
#include <vector>

#include <SFML/System/Vector2.hpp>

/// One elevator route declared in the external config, in grid coordinates.
struct ElevatorRouteConfig {
    sf::Vector2i start;   ///< grid column/row of the platform start
    sf::Vector2i end;     ///< grid column/row of the platform end
    float speedPixelsPerSecond;
    float pauseSeconds;
};

namespace ElevatorConfig {

/// Routes for the given level file (e.g. "levels/level2.txt" or "level2.txt").
/// Returns an empty vector when the level has no declared routes.
const std::vector<ElevatorRouteConfig>& routesFor(const std::string& levelFilePath);

} // namespace ElevatorConfig