/**
 * @file CharacterFlowTests.cpp
 * @brief Independent behavior checks for character selection and identity flow.
 */

#include <cassert>
#include <cmath>
#include <filesystem>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "core/DisplayConfig.h"
#include "core/GameManager.h"
#include "core/TextureManager.h"
#include "entities/Mario.h"
#include "level/Level.h"
#include "patterns/InputState.h"
#include "states/CharacterSelectState.h"
#include "ui/UIMenuWidget.h"

namespace {

sf::Event keyPressed(sf::Keyboard::Key key) {
    return sf::Event(sf::Event::KeyPressed{
        key, sf::Keyboard::Scan::Unknown, false, false, false, false});
}

bool check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return false;
    }
    return true;
}

std::filesystem::path projectRoot() {
    auto root = std::filesystem::current_path();
    while (!std::filesystem::exists(root / "assets" / "fonts" / "mario.ttf") &&
           root != root.root_path()) {
        root = root.parent_path();
    }
    return root;
}

bool testImmutableProfilesAndDefaultCompatibility() {
    std::cout << "[RUNNING] testImmutableProfilesAndDefaultCompatibility...\n";
    constexpr CharacterProfile mario = characterProfileFor(CharacterType::MARIO);
    constexpr CharacterProfile luigi = characterProfileFor(CharacterType::LUIGI);

    return check(mario.jumpForce == 460.0f && mario.walkMaxSpeed == 175.0f &&
                     mario.runMaxSpeed == 280.0f &&
                     mario.underwaterWalkMaxSpeed == 100.0f &&
                     mario.underwaterRunMaxSpeed == 160.0f,
                 "Mario profile must retain all five baseline values") &&
           check(luigi.jumpForce == 510.0f && luigi.walkMaxSpeed == 160.0f &&
                     luigi.runMaxSpeed == 250.0f &&
                     luigi.underwaterWalkMaxSpeed == 90.0f &&
                     luigi.underwaterRunMaxSpeed == 144.0f,
                 "Luigi profile must expose the exact five contract values") &&
           check(Mario().getCharacterType() == CharacterType::MARIO &&
                     Mario().getJumpForce() == 460.0f,
                 "No-argument Mario construction must remain Mario-compatible");
}

bool testIdentitySurvivesLevelLoadAndSameLevelReload() {
    std::cout << "[RUNNING] testIdentitySurvivesLevelLoadAndSameLevelReload...\n";
    const auto root = projectRoot();
    if (!check(std::filesystem::exists(root / "levels" / "level1.txt"),
               "test must locate the level fixture")) {
        return false;
    }
    const auto original = std::filesystem::current_path();
    std::filesystem::current_path(root);

    Level level;
    bool ok = level.loadFromFile("levels/level1.txt", CharacterType::LUIGI);
    ok = ok && check(level.getCharacterType() == CharacterType::LUIGI,
                     "Level must report explicit Luigi identity immediately") &&
         check(level.getMario() &&
                   level.getMario()->getCharacterType() == CharacterType::LUIGI,
               "first spawned Mario must carry the chosen Luigi identity");
    level.update(0.0f);
    ok = ok && check(level.getMario()->getCharacterType() == CharacterType::LUIGI,
                     "identity must survive the first Level update");

    ok = ok && level.loadFromFile("levels/level1.txt", CharacterType::LUIGI) &&
         check(level.getCharacterType() == CharacterType::LUIGI &&
                   level.getMario()->getCharacterType() == CharacterType::LUIGI,
               "same-level reload must preserve the explicit identity");
    std::filesystem::current_path(original);
    return ok;
}

bool testMenuKeyboardAndLogicalMouseSelection() {
    std::cout << "[RUNNING] testMenuKeyboardAndLogicalMouseSelection...\n";
    sf::Font font;
    if (!check(font.openFromFile("assets/fonts/mario.ttf"),
               "selection test requires the packaged Mario font")) {
        return false;
    }

    int selected = 0;
    UIMenuWidget menu(font);
    menu.addItem("MARIO - FASTER RUNNER", [&] { selected = 1; });
    menu.addItem("LUIGI - HIGHER JUMPER", [&] { selected = 2; });
    menu.setSpacing(64.0f);
    menu.setPosition({DisplayConfig::LOGICAL_WIDTH / 2.0f, 178.0f},
                      UIAnchor::Center);

    InputState enter;
    enter.handleEvent(keyPressed(sf::Keyboard::Key::Enter));
    menu.processInput(enter);
    if (!check(selected == 1, "default Mario selection must activate with Enter")) {
        return false;
    }

    InputState down;
    down.handleEvent(keyPressed(sf::Keyboard::Key::Down));
    menu.processInput(down);
    InputState enterLuigi;
    enterLuigi.handleEvent(keyPressed(sf::Keyboard::Key::Enter));
    menu.processInput(enterLuigi);
    if (!check(selected == 2, "Down plus Enter must activate Luigi")) {
        return false;
    }

    const auto luigiBounds = menu.getItemBounds(1);
    if (!check(luigiBounds.has_value(), "Luigi menu item must expose logical bounds")) {
        return false;
    }
    const sf::Vector2f center = luigiBounds->position + luigiBounds->size / 2.0f;
    menu.processEvents(sf::Event(sf::Event::MouseButtonPressed{
        sf::Mouse::Button::Left,
        {static_cast<int>(center.x), static_cast<int>(center.y)}}));
    return check(selected == 2,
                 "logical mouse click on Luigi must activate Luigi");
}

sf::Image renderState(IGameState& state) {
    sf::RenderTexture texture;
    assert(texture.resize({DisplayConfig::LOGICAL_WIDTH,
                           DisplayConfig::LOGICAL_HEIGHT}));
    texture.clear(sf::Color::Transparent);
    state.render(texture);
    texture.display();
    return texture.getTexture().copyToImage();
}

bool testCharacterSelectRendersAndWritesArtifact() {
    std::cout << "[RUNNING] testCharacterSelectRendersAndWritesArtifact...\n";
    const auto root = projectRoot();
    const auto original = std::filesystem::current_path();
    std::filesystem::current_path(root);

    CharacterSelectState state;
    state.onEnter();
    const sf::Image image = renderState(state);
    const auto artifact = std::filesystem::temp_directory_path() /
                          "character_select_t1.png";
    const bool saved = image.saveToFile(artifact.string());

    std::size_t titlePixels = 0;
    std::size_t detailPixels = 0;
    std::size_t outsidePanelInk = 0;
    for (unsigned int y = 35; y < 70; ++y) {
        for (unsigned int x = 50; x < 590; ++x) {
            const sf::Color pixel = image.getPixel({x, y});
            if (pixel.r > 180 && pixel.g > 130 && pixel.b < 180) {
                ++titlePixels;
            }
        }
    }
    for (unsigned int y = 112; y < 263; ++y) {
        for (unsigned int x = 80; x < 560; ++x) {
            const sf::Color pixel = image.getPixel({x, y});
            if (pixel.r > 150 && pixel.g > 120 && pixel.b < 190) {
                ++detailPixels;
            }
        }
    }
    for (unsigned int y = 70; y < 96; ++y) {
        for (unsigned int x = 0; x < 640; ++x) {
            if (x >= 45 && x < 595) {
                continue;
            }
            if (image.getPixel({x, y}) != sf::Color(28, 42, 82)) {
                ++outsidePanelInk;
            }
        }
    }
    for (unsigned int y = 312; y < 339; ++y) {
        for (unsigned int x = 0; x < 640; ++x) {
            if (x >= 45 && x < 595) {
                continue;
            }
            if (image.getPixel({x, y}) != sf::Color(28, 42, 82)) {
                ++outsidePanelInk;
            }
        }
    }
    state.onExit();
    std::filesystem::current_path(original);

    std::cout << "[ARTIFACT] " << artifact.string() << '\n';
    std::cout << "[RENDER] titlePixels=" << titlePixels
              << " detailPixels=" << detailPixels
              << " outsidePanelInk=" << outsidePanelInk << '\n';
    return check(saved, "CharacterSelect must render to a PNG artifact") &&
           check(image.getPixel({55, 25}) == sf::Color(17, 24, 52),
                 "CharacterSelect panel must be visible in the default view") &&
           check(titlePixels > 20, "CharacterSelect title must be visibly rendered") &&
           check(detailPixels > 20,
                 "CharacterSelect cards must contain visible hero/details text") &&
           check(outsidePanelInk == 0,
                 "CharacterSelect instructional text must fit inside the panel");
}

bool testDuplicateEnterQueuesOnlyOneTransition() {
    std::cout << "[RUNNING] testDuplicateEnterQueuesOnlyOneTransition...\n";
    const auto root = projectRoot();
    const auto original = std::filesystem::current_path();
    std::filesystem::current_path(root);

    GameManager& game = GameManager::getInstance();
    game.changeState(std::make_unique<CharacterSelectState>());
    game.update(0.0f);
    std::cout << "[TRACE] CharacterSelect entered\n";

    InputState enter;
    enter.handleEvent(keyPressed(sf::Keyboard::Key::Enter));
    game.processInput(enter);
    game.processInput(enter);
    std::cout << "[TRACE] duplicate input dispatched\n";
    game.update(0.0f);
    std::cout << "[TRACE] PlayState transition applied\n";

    // A duplicate activation must not leave multiple pending PlayStates.
    const bool oneState = game.stackDepth() == 1;
    // Leave a non-EventBus state on the singleton stack so static teardown
    // does not destroy PlayState after EventBus's static lifetime ends.
    game.changeState(std::make_unique<CharacterSelectState>());
    game.update(0.0f);
    std::filesystem::current_path(original);
    return check(oneState,
                 "duplicate Enter activation must produce one deferred transition");
}

} // namespace

int main() {
    const auto root = projectRoot();
    const auto original = std::filesystem::current_path();
    std::filesystem::current_path(root);

    const bool ok = testImmutableProfilesAndDefaultCompatibility() &&
                    testIdentitySurvivesLevelLoadAndSameLevelReload() &&
                    testMenuKeyboardAndLogicalMouseSelection() &&
                    testCharacterSelectRendersAndWritesArtifact() &&
                    testDuplicateEnterQueuesOnlyOneTransition();

    // TextureManager is a process-lifetime singleton. Release its SFML
    // textures explicitly while SFML's context subsystem is still valid.
    TextureManager::getInstance().shutdown();

    std::filesystem::current_path(original);
    return ok ? 0 : 1;
}
