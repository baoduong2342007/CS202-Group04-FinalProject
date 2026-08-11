/**
 * @file TV5IntegrationTests.cpp
 * @brief Regression coverage for TV5 input, item, HUD, audio and asset work.
 */

#include <array>
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <string>
#include <utility>

#include "core/SaveManager.h"
#include "core/ScoreRules.h"
#include "core/SoundManager.h"
#include "core/LevelCatalog.h"
#include "entities/Mario.h"
#include "entities/QuestionBlock.h"
#include "items/Coin.h"
#include "items/FireFlower.h"
#include "items/Mushroom.h"
#include "items/Star.h"
#include "patterns/EntityFactory.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "patterns/IObserver.h"
#include "patterns/InputHandler.h"
#include "patterns/InputState.h"
#include "patterns/RunCommand.h"
#include "patterns/ShootCommand.h"
#include "ui/HUD.h"

namespace {

class EventCounter final : public IObserver {
public:
    EventCounter() {
        EventBus::getInstance().subscribe(EventType::PLAYER_POWER_UP, this);
        EventBus::getInstance().subscribe(EventType::ONE_UP_COLLECTED, this);
        EventBus::getInstance().subscribe(EventType::PLAYER_INVINCIBILITY_EXPIRED, this);
    }

    ~EventCounter() override {
        EventBus::getInstance().unsubscribe(EventType::PLAYER_POWER_UP, this);
        EventBus::getInstance().unsubscribe(EventType::ONE_UP_COLLECTED, this);
        EventBus::getInstance().unsubscribe(EventType::PLAYER_INVINCIBILITY_EXPIRED, this);
    }

    void onNotify(EventType event) override {
        if (event == EventType::PLAYER_POWER_UP) {
            ++powerUpEvents;
        } else if (event == EventType::ONE_UP_COLLECTED) {
            ++oneUpEvents;
        } else if (event == EventType::PLAYER_INVINCIBILITY_EXPIRED) {
            ++invincibilityExpiredEvents;
        }
    }

    int powerUpEvents = 0;
    int oneUpEvents = 0;
    int invincibilityExpiredEvents = 0;
};

sf::Event keyPressed(sf::Keyboard::Key key) {
    return sf::Event(sf::Event::KeyPressed{
        key, sf::Keyboard::Scan::Unknown, false, false, false, false});
}

sf::Event keyReleased(sf::Keyboard::Key key) {
    return sf::Event(sf::Event::KeyReleased{
        key, sf::Keyboard::Scan::Unknown, false, false, false, false});
}

std::filesystem::path projectRoot() {
    std::filesystem::path candidate = std::filesystem::current_path();
    for (int depth = 0; depth < 8; ++depth) {
        if (std::filesystem::exists(candidate / "assets" / "ASSETS_LIST.md") &&
            std::filesystem::exists(candidate / "src")) {
            return candidate;
        }

        const std::filesystem::path parent = candidate.parent_path();
        if (parent == candidate) {
            break;
        }
        candidate = parent;
    }

    return std::filesystem::current_path();
}

std::pair<unsigned int, unsigned int> pngDimensions(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    assert(input.is_open());

    std::array<unsigned char, 24> header{};
    input.read(reinterpret_cast<char*>(header.data()), static_cast<std::streamsize>(header.size()));
    assert(input.gcount() == static_cast<std::streamsize>(header.size()));

    constexpr std::array<unsigned char, 8> PNG_SIGNATURE{
        0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
    assert(std::equal(PNG_SIGNATURE.begin(), PNG_SIGNATURE.end(), header.begin()));

    const auto readBigEndian = [&header](std::size_t offset) {
        return (static_cast<unsigned int>(header[offset]) << 24u) |
               (static_cast<unsigned int>(header[offset + 1]) << 16u) |
               (static_cast<unsigned int>(header[offset + 2]) << 8u) |
               static_cast<unsigned int>(header[offset + 3]);
    };

    return {readBigEndian(16), readBigEndian(20)};
}

void testScoreCatalogAndCoinThreshold() {
    assert(ScoreRules::pointsFor(ScoreEvent::COIN_COLLECTED) == 100);
    assert(ScoreRules::pointsFor(ScoreEvent::POWER_UP_COLLECTED) == 1000);
    assert(ScoreRules::pointsFor(ScoreEvent::ENEMY_STOMPED) == 100);
    assert(ScoreRules::pointsFor(ScoreEvent::SHELL_DEFEATED) == 200);
    assert(ScoreRules::pointsFor(ScoreEvent::FIREBALL_DEFEATED) == 200);
    assert(ScoreRules::pointsFor(ScoreEvent::STAR_DEFEATED) == 200);
    assert(ScoreRules::pointsFor(DefeatCause::STOMP) == 100);
    assert(ScoreRules::pointsFor(DefeatCause::SHELL) == 200);
    assert(ScoreRules::pointsFor(DefeatCause::FIREBALL) == 200);
    assert(ScoreRules::pointsFor(DefeatCause::STAR) == 200);
    assert(ScoreRules::pointsFor(DefeatCause::PIT) == 0);

    EventCounter events;
    Mario mario;
    mario.setCoinCount(99);
    const int initialLives = mario.getLives();

    Coin::awardTo(mario);

    assert(mario.getScore() == 100);
    assert(mario.getCoinCount() == 0);
    assert(mario.getLives() == initialLives + 1);
    assert(events.oneUpEvents == 1);

    Mario defeatMario;
    ScoreRules::awardDefeat(defeatMario, DefeatCause::STOMP);
    ScoreRules::awardDefeat(defeatMario, DefeatCause::SHELL);
    ScoreRules::awardDefeat(defeatMario, DefeatCause::FIREBALL);
    ScoreRules::awardDefeat(defeatMario, DefeatCause::STAR);
    ScoreRules::awardDefeat(defeatMario, DefeatCause::PIT);
    assert(defeatMario.getScore() == 700);
}

void testPowerUpAndOneUpEvents() {
    EventCounter events;
    Mario mario;
    const int initialLives = mario.getLives();

    FireFlower fireFlower;
    fireFlower.onCollect(mario);
    assert(mario.getMarioState() == MarioState::FIRE);
    assert(mario.canShootFireBall());
    assert(mario.getScore() == 1000);
    assert(events.powerUpEvents == 1);

    Mushroom oneUp(MushroomType::ONE_UP);
    oneUp.onCollect(mario);
    assert(mario.getLives() == initialLives + 1);
    assert(mario.getScore() == 2000);
    assert(events.oneUpEvents == 1);
    assert(events.powerUpEvents == 1);
}

void testAdaptiveQuestionBlockAndFireFlowerContract() {
    std::unique_ptr<Entity> factoryBlock =
        EntityFactory::createFromTileCode('?', {0.f, 0.f}, nullptr);
    auto* adaptiveBlock = dynamic_cast<QuestionBlock*>(factoryBlock.get());
    assert(adaptiveBlock != nullptr);
    assert(adaptiveBlock->getContent() == QuestionBlockContent::ADAPTIVE);

    Mario smallMario;
    adaptiveBlock->onHit(smallMario);
    assert(adaptiveBlock->getContent() == QuestionBlockContent::SUPER_MUSHROOM);
    adaptiveBlock->onHit(smallMario);
    assert(adaptiveBlock->getContent() == QuestionBlockContent::SUPER_MUSHROOM);

    QuestionBlock poweredBlock(
        {0.f, 0.f}, nullptr, QuestionBlockContent::ADAPTIVE);
    Mario superMario;
    superMario.setMarioState(MarioState::SUPER);
    poweredBlock.onHit(superMario);
    assert(poweredBlock.getContent() == QuestionBlockContent::FIRE_FLOWER);

    QuestionBlock fireBlock(
        {0.f, 0.f}, nullptr, QuestionBlockContent::ADAPTIVE);
    Mario fireMario;
    fireMario.setMarioState(MarioState::FIRE);
    fireBlock.onHit(fireMario);
    assert(fireBlock.getContent() == QuestionBlockContent::FIRE_FLOWER);

    FireFlower directFlower;
    Mario directMario;
    directMario.setMarioState(MarioState::SUPER);
    directFlower.onCollect(directMario);
    assert(directMario.getMarioState() == MarioState::FIRE);
    assert(directMario.getScore() == 1000);
    directFlower.onCollect(directMario);
    assert(directMario.getScore() == 1000); // one item, one award
}

void testStarTimerHudAndExpiryEvent() {
    EventCounter events;
    Mario mario;
    HUD hud(mario);
    Star star;

    star.onCollect(mario);
    assert(mario.isStarInvincible());
    assert(hud.getPowerLabel() == "STAR");

    mario.updateInvincibility(9.99f);
    assert(mario.isStarInvincible());
    assert(events.invincibilityExpiredEvents == 0);

    mario.updateInvincibility(0.02f);
    assert(!mario.isStarInvincible());
    assert(events.invincibilityExpiredEvents == 1);
    assert(hud.getPowerLabel() == "SMALL");

    // Expiry is an edge, not a level-triggered event.
    mario.updateInvincibility(1.f);
    assert(events.invincibilityExpiredEvents == 1);
}

void testHudTimeoutAndGameplayFreeze() {
    Mario mario;
    HUD hud(mario);
    assert(hud.getTimeRemaining() == HUD::DEFAULT_LEVEL_TIME);
    int warnings = 0;
    int timeouts = 0;
    hud.setTimeWarningCallback([&warnings] { ++warnings; });
    hud.setTimeoutCallback([&timeouts] { ++timeouts; });

    hud.resetTimer(101);
    hud.update(1.f, true);
    assert(hud.getTimeRemaining() == 100);
    assert(warnings == 1);

    hud.update(100.f, true);
    assert(hud.getTimeRemaining() == 0);
    assert(timeouts == 1);

    hud.update(10.f, true);
    assert(timeouts == 1);

    hud.resetTimer(10);
    EventBus::getInstance().notify(EventType::GAME_PAUSED);
    hud.update(5.f, true);
    assert(hud.getTimeRemaining() == 10);

    hud.update(2.f, false);
    assert(hud.getTimeRemaining() == 10);

    hud.resetTimer(10);
    EventBus::getInstance().notify(EventType::LEVEL_COMPLETED);
    hud.update(20.f, true);
    assert(hud.getTimeRemaining() == 10);
    assert(timeouts == 1);

    hud.resetTimer(10);
    EventBus::getInstance().notify(EventType::PLAYER_DIED);
    hud.update(20.f, true);
    assert(hud.getTimeRemaining() == 10);
    assert(timeouts == 1);
}

void testInputBindingsAndSuppression() {
    InputState input;
    InputHandler handler;
    int runRequests = 0;
    int shootRequests = 0;

    handler.bindKey(
        sf::Keyboard::Key::LShift,
        std::make_unique<RunCommand>([&runRequests] { ++runRequests; }),
        InputTrigger::Held);
    handler.bindKey(
        sf::Keyboard::Key::X,
        std::make_unique<ShootCommand>([&shootRequests] { ++shootRequests; }),
        InputTrigger::Pressed);

    input.beginFrame();
    input.handleEvent(keyPressed(sf::Keyboard::Key::LShift));
    input.handleEvent(keyPressed(sf::Keyboard::Key::X));
    handler.handleInput(input);
    assert(runRequests == 1);
    assert(shootRequests == 1);

    input.beginFrame();
    input.handleEvent(keyPressed(sf::Keyboard::Key::LShift));
    input.handleEvent(keyPressed(sf::Keyboard::Key::X));
    handler.handleInput(input);
    assert(runRequests == 2);
    assert(shootRequests == 1); // X auto-repeat is not a new Pressed edge.

    input.beginFrame();
    input.handleEvent(keyReleased(sf::Keyboard::Key::LShift));
    input.handleEvent(keyReleased(sf::Keyboard::Key::X));
    handler.handleInput(input);
    assert(runRequests == 2);

    input.beginFrame();
    input.handleEvent(keyPressed(sf::Keyboard::Key::X));
    handler.handleInput(input, false); // Pause/death/transition suppression.
    assert(shootRequests == 1);
}

void testStarMusicOverrideAndVolumePersistence() {
    SoundManager& sound = SoundManager::getInstance();
    assert(LevelCatalog::find(1)->music == MusicId::OVERWORLD);
    assert(LevelCatalog::find(2)->music == MusicId::UNDERGROUND);
    assert(LevelCatalog::find(3)->music == MusicId::CASTLE);

    for (const char* effect : {"coin", "stomp", "kick", "death", "powerup",
                               "powerdown", "fireball", "flagpole", "oneup",
                               "hurryup"}) {
        assert(sound.isSoundLoaded(effect));
    }

    sound.setLevelMusic(MusicId::UNDERGROUND);
    sound.playMusic(MusicId::UNDERGROUND);
    sound.playStarMusic();

    // The test runs from a clean CMake output where the music assets are
    // copied. The ID assertions verify the override/restore lifecycle rather
    // than audio-device state.
    assert(sound.isStarMusicActive());
    assert(sound.getCurrentMusicId().has_value());
    assert(sound.getCurrentMusicId().value() == MusicId::STAR);

    EventBus::getInstance().notify(EventType::PLAYER_INVINCIBILITY_EXPIRED);
    assert(!sound.isStarMusicActive());
    assert(sound.getCurrentMusicId().has_value());
    assert(sound.getCurrentMusicId().value() == MusicId::UNDERGROUND);

    const std::filesystem::path saveDirectory =
        std::filesystem::temp_directory_path() / "tv5_audio_persistence_tests";
    const std::filesystem::path savePath = saveDirectory / "save.txt";
    std::error_code errorCode;
    std::filesystem::remove_all(saveDirectory, errorCode);

    SaveManager manager(savePath.string());
    assert(manager.updateAudioSettings(33.f, 44.f));

    SaveManager reloaded(savePath.string());
    assert(reloaded.load());
    assert(reloaded.getData().soundVolume == 33.f);
    assert(reloaded.getData().musicVolume == 44.f);

    std::filesystem::remove_all(saveDirectory, errorCode);
}

void testVolumeClampAndAssetManifest() {
    assert(SoundManager::clampVolume(-10.f) == 0.f);
    assert(SoundManager::clampVolume(150.f) == 100.f);
    assert(SoundManager::clampVolume(std::numeric_limits<float>::quiet_NaN()) == 0.f);

    const std::filesystem::path root = projectRoot();
    const auto assertDimensions = [&root](const char* relativePath,
                                          unsigned int width,
                                          unsigned int height) {
        const std::filesystem::path path = root / relativePath;
        assert(std::filesystem::exists(path));
        const auto dimensions = pngDimensions(path);
        assert(dimensions.first == width);
        assert(dimensions.second == height);
    };

    assertDimensions("assets/textures/tiles/tileset.png", 680, 356);
    assertDimensions("assets/textures/enemies/enemies.png", 436, 530);
    assertDimensions("assets/textures/enemies/goomba.png", 96, 32);
    assertDimensions("assets/textures/enemies/koopa.png", 128, 48);
    assertDimensions("assets/textures/items/items_blocks.png", 448, 256);
    assertDimensions("assets/textures/items/items_objects.png", 592, 572);

    std::ifstream manifest(root / "assets/ASSETS_LIST.md");
    assert(manifest.is_open());
    const std::string manifestText(
        (std::istreambuf_iterator<char>(manifest)), std::istreambuf_iterator<char>());
    assert(manifestText.find("assets/textures/tiles/tileset.png") != std::string::npos);
    assert(manifestText.find("680×776") != std::string::npos);
    assert(manifestText.find("assets/textures/enemies/enemies.png") != std::string::npos);
    assert(manifestText.find("436×530") != std::string::npos);
    assert(manifestText.find("docs/assets/reference/blocks_all_components_atlas_full.png") !=
           std::string::npos);

    // Analysis/source atlases are documentation assets, not runtime package
    // inputs. Their canonical copies live below docs/assets/reference.
    assert(!std::filesystem::exists(
        root / "assets/textures/enemies/enemies_all_components_atlas.png"));
    assert(!std::filesystem::exists(
        root / "assets/textures/items/blocks_all_components_atlas.png"));
    assert(!std::filesystem::exists(
        root / "assets/textures/items/items_objects_all_components_atlas.png"));
    assert(std::filesystem::exists(
        root / "docs/assets/reference/enemies_all_components_atlas_full.png"));
    assert(std::filesystem::exists(
        root / "docs/assets/reference/blocks_all_components_atlas_full.png"));
    assert(std::filesystem::exists(
        root / "docs/assets/reference/items_objects_all_components_atlas_full.png"));
}

} // namespace

int main() {
    testScoreCatalogAndCoinThreshold();
    testPowerUpAndOneUpEvents();
    testAdaptiveQuestionBlockAndFireFlowerContract();
    testStarTimerHudAndExpiryEvent();
    testHudTimeoutAndGameplayFreeze();
    testInputBindingsAndSuppression();
    testStarMusicOverrideAndVolumePersistence();
    testVolumeClampAndAssetManifest();
    std::cout << "All TV5 integration tests passed successfully!\n";
    return 0;
}
