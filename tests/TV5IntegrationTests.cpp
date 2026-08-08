/**
 * @file TV5IntegrationTests.cpp
 * @brief Focused regression coverage for the reopened TV5 gameplay/audio/HUD work.
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>

#include "core/ScoreRules.h"
#include "core/SoundManager.h"
#include "entities/Mario.h"
#include "items/Coin.h"
#include "items/FireFlower.h"
#include "items/Mushroom.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "patterns/IObserver.h"
#include "ui/HUD.h"

namespace {

class EventCounter final : public IObserver {
public:
    EventCounter() {
        EventBus::getInstance().subscribe(EventType::PLAYER_POWER_UP, this);
        EventBus::getInstance().subscribe(EventType::ONE_UP_COLLECTED, this);
    }

    ~EventCounter() override {
        EventBus::getInstance().unsubscribe(EventType::PLAYER_POWER_UP, this);
        EventBus::getInstance().unsubscribe(EventType::ONE_UP_COLLECTED, this);
    }

    void onNotify(EventType event) override {
        if (event == EventType::PLAYER_POWER_UP) {
            ++powerUpEvents;
        } else if (event == EventType::ONE_UP_COLLECTED) {
            ++oneUpEvents;
        }
    }

    int powerUpEvents = 0;
    int oneUpEvents = 0;
};

void testScoreCatalogAndCoinThreshold() {
    assert(ScoreRules::pointsFor(ScoreEvent::COIN_COLLECTED) == 100);
    assert(ScoreRules::pointsFor(ScoreEvent::POWER_UP_COLLECTED) == 1000);
    assert(ScoreRules::pointsFor(ScoreEvent::ENEMY_STOMPED) == 100);
    assert(ScoreRules::pointsFor(ScoreEvent::SHELL_DEFEATED) == 200);
    assert(ScoreRules::pointsFor(ScoreEvent::FIREBALL_DEFEATED) == 200);
    assert(ScoreRules::pointsFor(ScoreEvent::STAR_DEFEATED) == 200);

    Mario mario;
    mario.setCoinCount(99);
    const int initialLives = mario.getLives();

    Coin::awardTo(mario);

    assert(mario.getScore() == 100);
    assert(mario.getCoinCount() == 0);
    assert(mario.getLives() == initialLives + 1);
}

void testPowerUpAndOneUpEvents() {
    EventCounter events;
    Mario mario;
    const int initialLives = mario.getLives();

    FireFlower fireFlower;
    fireFlower.onCollect(mario);
    assert(mario.getMarioState() == MarioState::FIRE || mario.getMarioState() == MarioState::FIRE_SMALL);
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

void testHudPowerLabelAndVolumeClamp() {
    Mario mario;
    HUD hud(mario);

    assert(hud.getPowerLabel() == "SMALL");
    EventBus::getInstance().notify(EventType::PLAYER_STAR_COLLECTED);
    assert(hud.getPowerLabel() == "STAR");
    EventBus::getInstance().notify(EventType::PLAYER_INVINCIBILITY_EXPIRED);
    assert(hud.getPowerLabel() == "SMALL");

    assert(SoundManager::clampVolume(-10.f) == 0.f);
    assert(SoundManager::clampVolume(150.f) == 100.f);
    assert(SoundManager::clampVolume(std::numeric_limits<float>::quiet_NaN()) == 0.f);
}

} // namespace

int main() {
    testScoreCatalogAndCoinThreshold();
    testPowerUpAndOneUpEvents();
    testHudPowerLabelAndVolumeClamp();
    std::cout << "All TV5 integration tests passed successfully!\n";
    return 0;
}
