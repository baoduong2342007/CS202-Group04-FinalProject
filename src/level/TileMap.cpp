/**
 * @file TileMap.cpp
 * @author TV4 (Vy)
 * @brief Implementation of TileMap file loading
 * @note Week 2 TileMap parsing
 */

#include "level/TileMap.h"

#include <fstream>
#include <iostream>
#include <string>

bool TileMap::loadFromFile(const std::string& path){
    std::ifstream inputFile(path);
    
    if (!inputFile.is_open()){
        std::cerr << "Failed to open level file: " << path << std::endl;
        return false;
    }
    
    m_grid.clear();
    
    std::string line;
    
    while(std::getline(inputFile, line)){
        if (line.empty() or line.front() == '#'){
            continue;
        }
        
        m_grid.push_back(line);
    }
    
    if (m_grid.empty()){
        std::cerr << "Level file contains no map data: " << path << std::endl;
        return false;
    }
    
    const std::size_t expectedWidth = m_grid.front().size();
    
    for (const std::string& row : m_grid){
        if (row.size() != expectedWidth){
            std::cerr << "Invalid level file: inconsistent row width in " << path << std::endl;
            m_grid.clear();
            return false;
        }
    }
    
    return true;
}
