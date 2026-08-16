#include <array>
#include <cassert>
#include <iostream>

#include "core/SpriteFrames_shared.h"
#include "entities/Goomba.h"
#include "entities/Koopa.h"
#include "entities/Mario.h"
#include "entities/ScorePopup.h"
#include "physics/CollisionManager.h"

namespace {

bool sameRect(const sf::IntRect& lhs, const sf::IntRect& rhs) {
    return lhs.position == rhs.position && lhs.size == rhs.size;
}

void testSharedFrameMapping() {
    using namespace SpriteFrames::shared::Items;
    assert(sameRect(ScorePopup::frameFor(100), SCORE_100));
    assert(sameRect(ScorePopup::frameFor(200), SCORE_200));
    assert(sameRect(ScorePopup::frameFor(400), SCORE_400));
    assert(sameRect(ScorePopup::frameFor(800), SCORE_800));
    assert(sameRect(ScorePopup::frameFor(1000), SCORE_1000));
    assert(sameRect(ScorePopup::frameFor(2000), SCORE_2000));
    assert(sameRect(ScorePopup::frameFor(4000), SCORE_4000));
    assert(sameRect(ScorePopup::frameFor(5000), SCORE_5000));
    assert(sameRect(ScorePopup::frameFor(8000), SCORE_8000));
    assert(sameRect(ScorePopup::frameFor(0, true), SCORE_1UP));
}

void testSimultaneousStompsSkipOneStep() {
    Mario mario;

    const StompScoreAward first = mario.awardStompScore({10.f, 20.f});
    const StompScoreAward second = mario.awardStompScore({30.f, 40.f});

    assert(first.points == 100 && !first.grantsLife);
    assert(second.points == 400 && !second.grantsLife);
    assert(mario.getScore() == 500);

    const auto pending = mario.consumePendingStompScoreAwards();
    assert(pending.size() == 2);
    assert(pending[0].position == sf::Vector2f(10.f, 20.f));
    assert(pending[1].position == sf::Vector2f(30.f, 40.f));

    // Consuming closes the simultaneity window but preserves the airborne
    // chain, so the next stomp receives the following value.
    assert(mario.awardStompScore({}).points == 800);
    mario.consumePendingStompScoreAwards();

    mario.resetStompScoreChain();
    assert(mario.awardStompScore({}).points == 100);
}

void testSequentialChainEndsInOneUp() {
    Mario mario;
    const int initialLives = mario.getLives();
    constexpr std::array<int, 9> expected = {
        100, 200, 400, 800, 1000, 2000, 4000, 5000, 8000
    };

    int total = 0;
    for (int points : expected) {
        const StompScoreAward award = mario.awardStompScore({});
        assert(!award.grantsLife);
        assert(award.points == points);
        total += points;
        mario.consumePendingStompScoreAwards();
    }

    const StompScoreAward oneUp = mario.awardStompScore({});
    assert(oneUp.grantsLife);
    assert(oneUp.points == 0);
    assert(mario.getScore() == total);
    assert(mario.getLives() == initialLives + 1);
}

void testPopupMovesAndExpires() {
    ScorePopup popup({100.f, 100.f}, 400);
    popup.update(0.5f);
    assert(popup.getPosition().y < 100.f);
    assert(!popup.shouldRemove());
    popup.update(0.31f);
    assert(popup.shouldRemove());
}

void testFireballDefeatScoreAndPopup() {
    b2World world({0.f, 0.f});
    Mario mario;
    Goomba goomba({100.f, 150.f}, &world);

    const int initialScore = mario.getScore();
    const bool defeated = CollisionManager::defeatEnemy(goomba, DefeatCause::FIREBALL, &mario);
    assert(defeated);
    assert(goomba.isDead());
    assert(mario.getScore() == initialScore + 200);

    const auto pending = mario.consumePendingStompScoreAwards();
    assert(pending.size() == 1);
    assert(pending[0].points == 200);
    assert(!pending[0].grantsLife);
    assert(pending[0].position.x == 100.f + goomba.getSize().x / 2.f);
}

void testSlidingShellDefeatChainAndPopups() {
    b2World world({0.f, 0.f});
    Mario mario;
    const int initialLives = mario.getLives();

    constexpr std::array<int, 8> expectedShellScores = {
        200, 400, 800, 1000, 2000, 4000, 5000, 8000
    };

    int total = 0;
    for (std::size_t i = 0; i < expectedShellScores.size(); ++i) {
        Goomba victim({static_cast<float>(i * 50), 100.f}, &world);
        const bool defeated = CollisionManager::defeatEnemy(
            victim, DefeatCause::SHELL, &mario, static_cast<int>(i));
        assert(defeated);
        assert(victim.isDead());

        total += expectedShellScores[i];
        assert(mario.getScore() == total);

        const auto pending = mario.consumePendingStompScoreAwards();
        assert(pending.size() == 1);
        assert(pending[0].points == expectedShellScores[i]);
        assert(!pending[0].grantsLife);
    }

    // 9th kill by sliding shell grants 1UP!
    Goomba ninthVictim({500.f, 100.f}, &world);
    const bool ninthDefeated = CollisionManager::defeatEnemy(
        ninthVictim, DefeatCause::SHELL, &mario, 8);
    assert(ninthDefeated);
    assert(mario.getLives() == initialLives + 1);

    const auto pendingOneUp = mario.consumePendingStompScoreAwards();
    assert(pendingOneUp.size() == 1);
    assert(pendingOneUp[0].grantsLife);
    assert(pendingOneUp[0].points == 0);
}

void testStarAndBlockBumpScoreAndPopups() {
    b2World world({0.f, 0.f});
    Mario mario;

    // Star defeat
    Goomba starVictim({200.f, 200.f}, &world);
    bool defeated = CollisionManager::defeatEnemy(starVictim, DefeatCause::STAR, &mario);
    assert(defeated);
    assert(mario.getScore() == 200);
    auto pending = mario.consumePendingStompScoreAwards();
    assert(pending.size() == 1);
    assert(pending[0].points == 200);

    // Block bump defeat
    Goomba bumpVictim({300.f, 200.f}, &world);
    defeated = CollisionManager::defeatEnemy(bumpVictim, DefeatCause::BLOCK_BUMP, &mario);
    assert(defeated);
    assert(mario.getScore() == 300); // 200 + 100
    pending = mario.consumePendingStompScoreAwards();
    assert(pending.size() == 1);
    assert(pending[0].points == 100);
}

} // namespace

int main() {
    testSharedFrameMapping();
    testSimultaneousStompsSkipOneStep();
    testSequentialChainEndsInOneUp();
    testPopupMovesAndExpires();
    testFireballDefeatScoreAndPopup();
    testSlidingShellDefeatChainAndPopups();
    testStarAndBlockBumpScoreAndPopups();
    std::cout << "All stomp score tests passed successfully!\n";
    return 0;
}
