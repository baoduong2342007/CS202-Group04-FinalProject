/**
 * @file CheepCheepConfig.cpp
 * @author TV4 (Vy) & CS202 Team
 * @brief Loads the external Cheep Cheep registry (levels/cheep_cheep.txt)
 */

#include "level/CheepCheepConfig.h"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

namespace {

constexpr const char* CHEEP_CONFIG_PATH = "levels/cheep_cheep.txt";

std::string fileNameOf(std::string_view path) {
    const std::size_t slash = path.find_last_of("/\\");
    return std::string((slash == std::string_view::npos) ? path : path.substr(slash + 1));
}

struct LoadedConfig {
    std::map<std::string, std::vector<CheepCheepSpawnConfig>, std::less<>> spawns;
    std::map<std::string, std::vector<CheepCheepRouteConfig>, std::less<>> routes;
    std::map<std::string, std::vector<CheepCheepGeneratorConfig>, std::less<>> generators;
};

LoadedConfig loadConfig() {
    LoadedConfig result;

    std::ifstream input(CHEEP_CONFIG_PATH);
    if (!input.is_open()) {
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
        std::string entryType;

        if (!(stream >> levelFile >> entryType)) {
            continue;
        }

        const std::string key = fileNameOf(levelFile);

        if (entryType == "SWIM" || entryType == "SPAWN") {
            int col = 0, row = 0;
            if (!(stream >> col >> row)) {
                std::cerr << "CheepCheepConfig: invalid coordinates in line: " << line << std::endl;
                continue;
            }

            CheepCheepSpawnConfig spawnConfig;
            spawnConfig.gridPosition = {col, row};
            spawnConfig.behavior = CheepCheepBehavior::SWIMMING;

            std::string colorStr;
            if (stream >> colorStr) {
                if (colorStr == "RED" || colorStr == "red") {
                    spawnConfig.color = CheepCheepColor::RED;
                } else {
                    spawnConfig.color = CheepCheepColor::GREEN;
                }
            }

            std::string dirStr;
            if (stream >> dirStr) {
                if (dirStr == "RIGHT" || dirStr == "right") {
                    spawnConfig.direction = Direction::RIGHT;
                } else {
                    spawnConfig.direction = Direction::LEFT;
                }
            }

            result.spawns[key].push_back(spawnConfig);
        } else if (entryType == "ROUTE" || entryType == "PATROL") {
            int startCol = 0, startRow = 0, endCol = 0, endRow = 0;
            if (!(stream >> startCol >> startRow >> endCol >> endRow)) {
                std::cerr << "CheepCheepConfig: invalid route endpoints in line: " << line << std::endl;
                continue;
            }

            CheepCheepRouteConfig routeConfig;
            routeConfig.start = {startCol, startRow};
            routeConfig.end = {endCol, endRow};
            routeConfig.speedPixelsPerSecond = 40.0f;

            float speed = 40.0f;
            if (stream >> speed) {
                routeConfig.speedPixelsPerSecond = speed;
            }

            std::string colorStr;
            if (stream >> colorStr) {
                if (colorStr == "RED" || colorStr == "red") {
                    routeConfig.color = CheepCheepColor::RED;
                } else {
                    routeConfig.color = CheepCheepColor::GREEN;
                }
            }

            result.routes[key].push_back(routeConfig);
        } else if (entryType == "GENERATOR" || entryType == "JUMP_ZONE") {
            int startCol = 0, endCol = 0;
            if (!(stream >> startCol >> endCol)) {
                std::cerr << "CheepCheepConfig: invalid generator range in line: " << line << std::endl;
                continue;
            }

            CheepCheepGeneratorConfig genConfig;
            genConfig.startColumn = startCol;
            genConfig.endColumn = endCol;

            float interval = 1.5f;
            float minVx = -80.f, maxVx = 80.f;
            float minVy = -700.f, maxVy = -550.f;

            if (stream >> interval) genConfig.intervalSeconds = interval;
            if (stream >> minVx) genConfig.minVx = minVx;
            if (stream >> maxVx) genConfig.maxVx = maxVx;
            if (stream >> minVy) genConfig.minVy = minVy;
            if (stream >> maxVy) genConfig.maxVy = maxVy;

            std::string colorStr;
            if (stream >> colorStr) {
                if (colorStr == "GREEN" || colorStr == "green") {
                    genConfig.color = CheepCheepColor::GREEN;
                } else {
                    genConfig.color = CheepCheepColor::RED;
                }
            }

            result.generators[key].push_back(genConfig);
        }
    }

    return result;
}

} // namespace

namespace CheepCheepConfig {

const std::vector<CheepCheepSpawnConfig>& spawnsFor(std::string_view levelFilePath) {
    static const LoadedConfig config = loadConfig();
    const std::string key = fileNameOf(levelFilePath);
    const auto it = config.spawns.find(key);
    if (it == config.spawns.end()) {
        static const std::vector<CheepCheepSpawnConfig> empty;
        return empty;
    }
    return it->second;
}

const std::vector<CheepCheepRouteConfig>& routesFor(std::string_view levelFilePath) {
    static const LoadedConfig config = loadConfig();
    const std::string key = fileNameOf(levelFilePath);
    const auto it = config.routes.find(key);
    if (it == config.routes.end()) {
        static const std::vector<CheepCheepRouteConfig> empty;
        return empty;
    }
    return it->second;
}

const std::vector<CheepCheepGeneratorConfig>& generatorsFor(std::string_view levelFilePath) {
    static const LoadedConfig config = loadConfig();
    const std::string key = fileNameOf(levelFilePath);
    const auto it = config.generators.find(key);
    if (it == config.generators.end()) {
        static const std::vector<CheepCheepGeneratorConfig> empty;
        return empty;
    }
    return it->second;
}

} // namespace CheepCheepConfig
