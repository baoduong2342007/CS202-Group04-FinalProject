/**
 * @file main.cpp
 * @author Group 04
 * @brief Main entry point for Super Mario game
 */

#include "core/Game.h"
#include "core/TextureManager.h"

int main() {
    Game game;
    game.run();

    TextureManager::getInstance().shutdown();

    return 0;
}
