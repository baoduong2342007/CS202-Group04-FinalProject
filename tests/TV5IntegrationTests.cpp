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

#include <box2d/box2d.h>

#include "core/SaveManager.h"
#include "core/ScoreRules.h"
#include "core/SoundManager.h"
#include "core/TextureManager.h"
#include "core/LevelCatalog.h"
#include "core/GameManager.h"
#include "entities/FireBall.h"
#include "entities/Mario.h"
#include "entities/Goomba.h"
#include "entities/Koopa.h"
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
#include "physics/ContactListener.h"
#include "level/TileMap.h"
#include "states/GameOverState.h"
#include "states/MenuState.h"
#include "states/PlayState.h"
#include "states/PauseState.h"
#include "states/WinState.h"
#include "ui/HUD.h"

namespace {

class EventCounter final : public IObserver {
public:
    EventCounter() {
        EventBus::getInstance().subscribe(EventType::PLAYER_POWER_UP, this);
        EventBus::getInstance().subscribe(EventType::ONE_UP_COLLECTED, this);
        EventBus::getInstance().subscribe(EventType::PLAYER_INVINCIBILITY_EXPIRED, this);
        EventBus::getInstance().subscribe(EventType::ENEMY_STOMPED, this);
        EventBus::getInstance().subscribe(EventType::SHELL_KICKED, this);
        EventBus::getInstance().subscribe(EventType::ENEMY_DEFEATED_BY_SHELL, this);
        EventBus::getInstance().subscribe(EventType::ENEMY_DEFEATED_BY_FIREBALL, this);
        EventBus::getInstance().subscribe(EventType::ENEMY_DEFEATED_BY_STAR, this);
        EventBus::getInstance().subscribe(EventType::COIN_COLLECTED, this);
    }

    ~EventCounter() override {
        EventBus::getInstance().unsubscribe(EventType::PLAYER_POWER_UP, this);
        EventBus::getInstance().unsubscribe(EventType::ONE_UP_COLLECTED, this);
        EventBus::getInstance().unsubscribe(EventType::PLAYER_INVINCIBILITY_EXPIRED, this);
        EventBus::getInstance().unsubscribe(EventType::ENEMY_STOMPED, this);
        EventBus::getInstance().unsubscribe(EventType::SHELL_KICKED, this);
        EventBus::getInstance().unsubscribe(EventType::ENEMY_DEFEATED_BY_SHELL, this);
        EventBus::getInstance().unsubscribe(EventType::ENEMY_DEFEATED_BY_FIREBALL, this);
        EventBus::getInstance().unsubscribe(EventType::ENEMY_DEFEATED_BY_STAR, this);
        EventBus::getInstance().unsubscribe(EventType::COIN_COLLECTED, this);
    }

    void onNotify(EventType event) override {
        if (event == EventType::PLAYER_POWER_UP) {
            ++powerUpEvents;
        } else if (event == EventType::ONE_UP_COLLECTED) {
            ++oneUpEvents;
        } else if (event == EventType::PLAYER_INVINCIBILITY_EXPIRED) {
            ++invincibilityExpiredEvents;
        } else if (event == EventType::ENEMY_STOMPED) {
            ++stompEvents;
        } else if (event == EventType::SHELL_KICKED) {
            ++shellKickedEvents;
        } else if (event == EventType::ENEMY_DEFEATED_BY_SHELL) {
            ++shellDefeatEvents;
        } else if (event == EventType::ENEMY_DEFEATED_BY_FIREBALL) {
            ++fireballDefeatEvents;
        } else if (event == EventType::ENEMY_DEFEATED_BY_STAR) {
            ++starDefeatEvents;
        } else if (event == EventType::COIN_COLLECTED) {
            ++coinCollectedEvents;
        }
    }

    int powerUpEvents = 0;
    int oneUpEvents = 0;
    int invincibilityExpiredEvents = 0;
    int stompEvents = 0;
    int shellKickedEvents = 0;
    int shellDefeatEvents = 0;
    int fireballDefeatEvents = 0;
    int starDefeatEvents = 0;
    int coinCollectedEvents = 0;
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
    assert(ScoreRules::pointsFor(ScoreEvent::BLOCK_BUMP_DEFEATED) == 100);
    assert(ScoreRules::pointsFor(DefeatCause::STOMP) == 100);
    assert(ScoreRules::pointsFor(DefeatCause::SHELL) == 200);
    assert(ScoreRules::pointsFor(DefeatCause::FIREBALL) == 200);
    assert(ScoreRules::pointsFor(DefeatCause::STAR) == 200);
    assert(ScoreRules::pointsFor(DefeatCause::BLOCK_BUMP) == 100);
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

}

void stepContactWorld(b2World& world) {
    world.Step(1.f / 60.f, 8, 3);
}

void testDefeatScoreAndShellEventsThroughCollisionRuntime() {
    // Stomp: the score/event producer is CollisionManager, not ScoreRules.
    {
        b2World world(b2Vec2(0.f, 0.f));
        TileMap tileMap;
        ContactListener listener(tileMap);
        world.SetContactListener(&listener);

        Mario mario({0.f, 0.f}, {28.f, 30.f});
        mario.initPhysics(&world, b2_dynamicBody, {28.f, 30.f});
        Goomba goomba({0.f, 25.f}, &world);
        EventCounter events;

        stepContactWorld(world);

        assert(goomba.isDead());
        assert(mario.getScore() == ScoreRules::pointsFor(DefeatCause::STOMP));
        assert(events.stompEvents == 1);

        // A second physics step keeps a persistent contact from awarding a
        // second score/event while the victim is awaiting cleanup.
        stepContactWorld(world);
        assert(mario.getScore() == 100);
        assert(events.stompEvents == 1);
    }

    // Shell kick and shell kill are distinct transactions. The victim is
    // scored through DefeatCause::SHELL and the first kick is not mislabeled
    // as ENEMY_STOMPED.
    {
        b2World world(b2Vec2(0.f, 0.f));
        TileMap tileMap;
        ContactListener listener(tileMap);
        world.SetContactListener(&listener);

        Mario owner;
        Koopa koopa({0.f, 0.f}, &world);
        Goomba victim({0.f, 0.f}, &world);
        EventCounter events;
        koopa.onStomp();
        koopa.update(0.f); // deferred shell fixture rebuild, world unlocked
        koopa.setDefeatOwner(&owner);
        koopa.kick(Direction::RIGHT);
        koopa.setVelocity({0.f, 0.f}); // keep the contact persistent for test

        stepContactWorld(world);
        assert(victim.isDead());
        assert(owner.getScore() == ScoreRules::pointsFor(DefeatCause::SHELL));
        assert(events.shellKickedEvents == 1);
        assert(events.shellDefeatEvents == 1);
        assert(events.stompEvents == 0);

        stepContactWorld(world);
        assert(owner.getScore() == 200);
        assert(events.shellDefeatEvents == 1);
        assert(events.stompEvents == 0);
        assert(events.shellKickedEvents == 1);
    }

    // FireBall and Star use the same shared operation and receive their own
    // cause event, rather than borrowing ENEMY_STOMPED.
    {
        b2World world(b2Vec2(0.f, 0.f));
        TileMap tileMap;
        ContactListener listener(tileMap);
        world.SetContactListener(&listener);

        Mario owner;
        FireBall fireBall({0.f, 0.f}, Direction::RIGHT, &world);
        fireBall.setOwner(&owner);
        fireBall.setVelocity({0.f, 0.f});
        Goomba goomba({0.f, 0.f}, &world);
        EventCounter events;

        stepContactWorld(world);
        assert(goomba.isDead());
        assert(owner.getScore() == ScoreRules::pointsFor(DefeatCause::FIREBALL));
        assert(events.fireballDefeatEvents == 1);
        assert(events.stompEvents == 0);
    }

    {
        b2World world(b2Vec2(0.f, 0.f));
        TileMap tileMap;
        ContactListener listener(tileMap);
        world.SetContactListener(&listener);

        Mario mario({0.f, 0.f}, {28.f, 30.f});
        mario.initPhysics(&world, b2_dynamicBody, {28.f, 30.f});
        mario.setStarInvincible(5.f);
        Goomba goomba({0.f, 25.f}, &world);
        EventCounter events;

        stepContactWorld(world);
        assert(goomba.isDead());
        assert(mario.getScore() == ScoreRules::pointsFor(DefeatCause::STAR));
        assert(events.starDefeatEvents == 1);
        assert(events.stompEvents == 0);
    }
}

void testPowerUpAndOneUpEvents() {
    EventCounter events;
    Mario mario;
    HUD hud(mario);
    const int initialLives = mario.getLives();

    FireFlower fireFlower;
    fireFlower.onCollect(mario);
    assert(mario.getMarioState() == MarioState::FIRE_SMALL);
    assert(mario.getSize().y == 30.f);
    assert(mario.canShootFireBall());
    assert(hud.getPowerLabel() == "FIRE SMALL");
    assert(mario.getScore() == 1000);
    assert(events.powerUpEvents == 1);

    FireFlower secondFireFlower;
    secondFireFlower.onCollect(mario);
    assert(mario.getMarioState() == MarioState::FIRE_SMALL);
    assert(mario.canShootFireBall());
    assert(hud.getPowerLabel() == "FIRE SMALL");
    assert(mario.getScore() == 2000);
    assert(events.powerUpEvents == 2);

    Mushroom oneUp(MushroomType::ONE_UP);
    oneUp.onCollect(mario);
    assert(mario.getLives() == initialLives + 1);
    assert(mario.getScore() == 3000);
    assert(events.oneUpEvents == 1);
    assert(events.powerUpEvents == 2);
}

void testAdaptiveQuestionBlockAndFireFlowerContract() {
    std::unique_ptr<Entity> factoryBlock =
        EntityFactory::createFromTileCode('?', {0.f, 0.f}, nullptr);
    auto* adaptiveBlock = dynamic_cast<QuestionBlock*>(factoryBlock.get());
    assert(adaptiveBlock != nullptr);
    assert(adaptiveBlock->getContent() == QuestionBlockContent::ADAPTIVE);

    assert(QuestionBlock::chooseRandomContent(0) == QuestionBlockContent::COIN);
    assert(QuestionBlock::chooseRandomContent(139) == QuestionBlockContent::COIN);
    assert(QuestionBlock::chooseRandomContent(140) ==
           QuestionBlockContent::SUPER_MUSHROOM);
    assert(QuestionBlock::chooseRandomContent(169) ==
           QuestionBlockContent::SUPER_MUSHROOM);
    assert(QuestionBlock::chooseRandomContent(170) ==
           QuestionBlockContent::FIRE_FLOWER);
    assert(QuestionBlock::chooseRandomContent(199) ==
           QuestionBlockContent::FIRE_FLOWER);

    Mario smallMario;
    adaptiveBlock->onHit(smallMario);
    const QuestionBlockContent smallMarioContent = adaptiveBlock->getContent();
    assert(smallMarioContent == QuestionBlockContent::COIN ||
           smallMarioContent == QuestionBlockContent::SUPER_MUSHROOM ||
           smallMarioContent == QuestionBlockContent::FIRE_FLOWER);
    const int coinsAfterFirstHit = smallMario.getCoinCount();
    adaptiveBlock->onHit(smallMario);
    assert(adaptiveBlock->getContent() == smallMarioContent);
    assert(smallMario.getCoinCount() == coinsAfterFirstHit);

    QuestionBlock poweredBlock(
        {0.f, 0.f}, nullptr, QuestionBlockContent::ADAPTIVE);
    Mario superMario;
    poweredBlock.onHit(superMario);
    assert(poweredBlock.getContent() == QuestionBlockContent::COIN ||
           poweredBlock.getContent() == QuestionBlockContent::SUPER_MUSHROOM ||
           poweredBlock.getContent() == QuestionBlockContent::FIRE_FLOWER);

    // An explicit 'f' tile spawns a FireFlower for Small Mario; collection
    // keeps the small body and selects the Small Fire sprite set.
    QuestionBlock explicitFlowerBlock(
        {0.f, 0.f}, nullptr, QuestionBlockContent::FIRE_FLOWER);
    Mario explicitSmallMario;
    std::vector<std::unique_ptr<Entity>> explicitSpawnedEntities;
    TextureManager& textureManager = TextureManager::getInstance();
    explicitFlowerBlock.onHit(
        explicitSmallMario, &explicitSpawnedEntities, &textureManager);
    assert(explicitFlowerBlock.getContent() ==
           QuestionBlockContent::FIRE_FLOWER);
    assert(explicitSpawnedEntities.size() == 1);
    assert(dynamic_cast<FireFlower*>(explicitSpawnedEntities.front().get()) !=
           nullptr);

    EventCounter coinEvents;
    Mario coinMario;
    QuestionBlock coinBlock(
        {0.f, 0.f}, nullptr, QuestionBlockContent::COIN);
    std::vector<std::unique_ptr<Entity>> spawnedEntities;
    coinBlock.onHit(coinMario, &spawnedEntities, &textureManager);
    assert(coinMario.getCoinCount() == 1);
    assert(coinMario.getScore() ==
           ScoreRules::pointsFor(ScoreEvent::COIN_COLLECTED));
    assert(coinEvents.coinCollectedEvents == 1);
    assert(spawnedEntities.size() == 1);
    auto* popupCoin = dynamic_cast<Coin*>(spawnedEntities.front().get());
    assert(popupCoin != nullptr);
    assert(popupCoin->getCoinType() == CoinType::QUESTION_POPUP);
    coinBlock.onHit(coinMario, &spawnedEntities, &textureManager);
    assert(coinMario.getCoinCount() == 1);
    assert(spawnedEntities.size() == 1);

    FireFlower directFlower;
    Mario directMario;
    directMario.setMarioState(MarioState::SUPER);
    directFlower.onCollect(directMario);
    assert(directMario.getMarioState() == MarioState::FIRE_SUPER);
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
    int verticalIntent = 0;

    handler.bindKey(
        sf::Keyboard::Key::LShift,
        std::make_unique<RunCommand>([&runRequests] { ++runRequests; }),
        InputTrigger::Held);
    handler.bindKey(
        sf::Keyboard::Key::X,
        std::make_unique<ShootCommand>([&shootRequests] { ++shootRequests; }),
        InputTrigger::Pressed);
    handler.bindKey(
        sf::Keyboard::Key::W,
        std::make_unique<RunCommand>([&verticalIntent] { verticalIntent = -1; }),
        InputTrigger::Held,
        InputGroup::Vertical);
    handler.bindKey(
        sf::Keyboard::Key::S,
        std::make_unique<RunCommand>([&verticalIntent] { verticalIntent = 1; }),
        InputTrigger::Held,
        InputGroup::Vertical);

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
    input.handleEvent(keyPressed(sf::Keyboard::Key::W));
    handler.handleInput(input);
    assert(verticalIntent == -1);

    // The latest vertical press wins while both keys remain held.
    input.beginFrame();
    input.handleEvent(keyPressed(sf::Keyboard::Key::S));
    handler.handleInput(input);
    assert(verticalIntent == 1);

    input.beginFrame();
    input.handleEvent(keyReleased(sf::Keyboard::Key::W));
    input.handleEvent(keyReleased(sf::Keyboard::Key::S));
    handler.handleInput(input);

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
    assert(LevelCatalog::find(4) == nullptr);

    for (const char* effect : {"coin", "stomp", "kick", "shell_kick",
                               "shell_kill", "enemy_fireball", "enemy_star",
                               "death", "powerup", "powerdown", "fireball",
                               "flagpole", "oneup", "hurryup"}) {
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

void testStateAudioRuntimeAndLevelTracks() {
    SoundManager& sound = SoundManager::getInstance();
    GameManager& game = GameManager::getInstance();

    // Start through the real GameManager -> PlayState lifecycle. Each
    // LEVEL_COMPLETED event below is consumed by PlayState's transition
    // transaction, which loads the next catalog entry and switches music.
    game.changeState(std::make_unique<PlayState>());
    game.update(0.f);
    assert(sound.getCurrentMusicId().has_value());
    assert(sound.getCurrentMusicId().value() == MusicId::OVERWORLD);

    // Death is an EventBus event consumed by SoundManager before PlayState
    // schedules its reload/GameOver decision.
    EventBus::getInstance().notify(EventType::PLAYER_DIED);
    assert(sound.getCurrentMusicId().has_value());
    assert(sound.getCurrentMusicId().value() == MusicId::DEATH);

    // Restart the state so the following checks begin at a clean Level 1.
    game.changeState(std::make_unique<PlayState>());
    game.update(0.f);
    assert(sound.getCurrentMusicId().value() == MusicId::OVERWORLD);

    for (int cycle = 0; cycle < 20; ++cycle) {
        game.pushState(std::make_unique<PauseState>());
        game.update(0.f);
        assert(sound.getCurrentMusicId().value() == MusicId::OVERWORLD);

        game.popState();
        game.update(0.f);
        assert(sound.getCurrentMusicId().value() == MusicId::OVERWORLD);
    }

    const auto completeCurrentLevel = [&game, &sound](MusicId nextTrack) {
        EventBus::getInstance().notify(EventType::LEVEL_COMPLETED);
        game.update(0.6f); // fade out -> loading
        game.update(0.f); // load next level, emit LEVEL_STARTED
        assert(sound.getCurrentMusicId().has_value());
        assert(sound.getCurrentMusicId().value() == nextTrack);
        game.update(0.6f); // fade in complete
    };

    completeCurrentLevel(MusicId::UNDERGROUND);
    completeCurrentLevel(MusicId::CASTLE);

    // Level 3 completion queues exactly one WinState at the safe point.
    EventBus::getInstance().notify(EventType::LEVEL_COMPLETED);
    game.update(0.6f);
    game.update(0.f);
    assert(sound.getCurrentMusicId().has_value());
    assert(sound.getCurrentMusicId().value() == MusicId::WIN);

    // GameOver/Win state entry points are also production audio state
    // lifecycles, not helper-only SoundManager calls.
    GameProgress progress;
    progress.score = 4321;
    GameOverState gameOver(progress);
    gameOver.onEnter();
    assert(sound.getCurrentMusicId().value() == MusicId::GAME_OVER);
    gameOver.onExit();

    WinState win(progress);
    win.onEnter();
    assert(sound.getCurrentMusicId().value() == MusicId::WIN);
    win.onExit();

    game.changeState(std::make_unique<MenuState>());
    game.update(0.f);
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
    assert(manifestText.find("680×356") != std::string::npos);
    assert(manifestText.find("assets/textures/enemies/enemies.png") != std::string::npos);
    assert(manifestText.find("436×530") != std::string::npos);
    assert(manifestText.find(
        "assets/textures/items/items_objects.png` | 592×572 | `Runtime` | Mushroom") !=
        std::string::npos);
    assert(manifestText.find(
        "assets/textures/items/items_blocks.png` | 448×256 | `Runtime` | QuestionBlock") !=
        std::string::npos);
    assert(manifestText.find(
        "assets/textures/ui/bg_clouds.png` | 768×1129 | `Future`") !=
        std::string::npos);
    assert(manifestText.find(
        "Tile catalog uses `assets/textures/tiles/tileset.png`") !=
        std::string::npos);
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
    testDefeatScoreAndShellEventsThroughCollisionRuntime();
    testPowerUpAndOneUpEvents();
    testAdaptiveQuestionBlockAndFireFlowerContract();
    testStarTimerHudAndExpiryEvent();
    testHudTimeoutAndGameplayFreeze();
    testInputBindingsAndSuppression();
    testStarMusicOverrideAndVolumePersistence();
    testStateAudioRuntimeAndLevelTracks();
    testVolumeClampAndAssetManifest();
    std::cout << "All TV5 integration tests passed successfully!\n";
    return 0;
}
