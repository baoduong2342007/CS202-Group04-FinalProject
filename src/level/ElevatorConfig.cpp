/**
 * @file ElevatorConfig.cpp
 * @author TV1 (Dương)
 * @brief Loads the external elevator-route registry (levels/elevators.txt)
 */

#include "level/ElevatorConfig.h"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

namespace {

constexpr const char* ELEVATOR_CONFIG_PATH = "levels/elevators.txt";

std::string fileNameOf(const std::string& path) {
    const std::size_t slash = path.find_last_of("/\\");
    return (slash == std::string::npos) ? path : path.substr(slash + 1);
}

std::map<std::string, std::vector<ElevatorRouteConfig>> loadConfig() {
    std::map<std::string, std::vector<ElevatorRouteConfig>> result;

    std::ifstream input(ELEVATOR_CONFIG_PATH);
    if (!input.is_open()) {
        // Missing config is not fatal: a level simply has no elevators.
        return result;
    }

    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream stream(line);
        std::string levelFile;
        int startCol, startRow, endCol, endRow;
        float speed = 60.0f;
        float pause = 1.0f;

        if (!(stream >> levelFile >> startCol >> startRow >> endCol >> endRow)) {
            std::cerr << "ElevatorConfig: skipping malformed line: " << line << std::endl;
            continue;
        }
        stream >> speed >> pause;

        if (startCol < 0 || startRow < 0 || endCol < 0 || endRow < 0) {
            std::cerr << "ElevatorConfig: negative grid coordinate in: " << line << std::endl;
            continue;
        }
        if (startCol == endCol && startRow == endRow) {
            std::cerr << "ElevatorConfig: route must span at least one tile: " << line << std::endl;
            continue;
        }
        if (startCol != endCol && startRow != endRow) {
            std::cerr << "ElevatorConfig: route must be vertical (same column) or "
                         "horizontal (same row): " << line << std::endl;
            continue;
        }

        result[fileNameOf(levelFile)].push_back(
            {{startCol, startRow}, {endCol, endRow}, speed, pause});
    }

    return result;
}

} // namespace

namespace ElevatorConfig {

const std::vector<ElevatorRouteConfig>& routesFor(const std::string& levelFilePath) {
    static const std::map<std::string, std::vector<ElevatorRouteConfig>> registry =
        loadConfig();

    const std::string key = fileNameOf(levelFilePath);
    const auto it = registry.find(key);
    if (it == registry.end()) {
        static const std::vector<ElevatorRouteConfig> empty;
        return empty;
    }
    return it->second;
}

} // namespace ElevatorConfig
