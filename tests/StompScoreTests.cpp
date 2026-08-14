#include <array>
#include <cassert>
#include <iostream>

#include "core/SpriteFrames_shared.h"
#include "entities/Mario.h"
#include "entities/ScorePopup.h"

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

} // namespace

int main() {
    testSharedFrameMapping();
    testSimultaneousStompsSkipOneStep();
    testSequentialChainEndsInOneUp();
    testPopupMovesAndExpires();
    std::cout << "All stomp score tests passed successfully!\n";
    return 0;
}
