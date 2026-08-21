/**
 * @file EventBusTests.cpp
 * @brief Focused coverage for value events and RAII subscription leases.
 */

#include <cassert>
#include <iostream>
#include <type_traits>
#include <utility>
#include <variant>

#include "patterns/EventBus.h"
#include "patterns/GameEvent.h"
#include "patterns/IObserver.h"
#include "patterns/Subscription.h"

namespace {

struct CountingObserver final : IObserver {
    void onNotify(const GameEvent& event) override {
        ++calls;
        lastEvent = event;
    }

    int calls = 0;
    GameEvent lastEvent{EventType::PLAYER_DIED};
};

struct RemovingObserver final : IObserver {
    RemovingObserver(EventBus& eventBus, Subscription& tokenToRemove)
        : bus(eventBus), target(tokenToRemove) {}

    void onNotify(const GameEvent& event) override {
        ++calls;
        target.reset();
        if (reenter) {
            reenter = false;
            bus.notify(event);
        }
    }

    EventBus& bus;
    Subscription& target;
    int calls = 0;
    bool reenter = false;
};

void testMoveOnlyTraits() {
    static_assert(!std::is_copy_constructible_v<Subscription>);
    static_assert(!std::is_copy_assignable_v<Subscription>);
    static_assert(std::is_move_constructible_v<Subscription>);
    static_assert(std::is_move_assignable_v<Subscription>);
    static_assert(std::is_same_v<decltype(GameEvent::context), EventContext>);

    std::cout << "[PASSED] testMoveOnlyTraits\n";
}

void testTemporaryTokenDisconnectsImmediately() {
    EventBus& bus = EventBus::getInstance();
    CountingObserver observer;

    bus.subscribe(EventType::PLAYER_DIED, &observer);
    bus.notify(EventType::PLAYER_DIED);
    assert(observer.calls == 0);

    std::cout << "[PASSED] testTemporaryTokenDisconnectsImmediately\n";
}

void testScopeMoveAndReset() {
    EventBus& bus = EventBus::getInstance();
    CountingObserver observer;

    Subscription token = bus.subscribe(EventType::PLAYER_DIED, &observer);
    assert(token.connected());
    Subscription moved = std::move(token);
    assert(!token.connected());
    assert(moved.connected());

    bus.notify(EventType::PLAYER_DIED);
    assert(observer.calls == 1);

    moved.reset();
    assert(!moved.connected());
    bus.notify(EventType::PLAYER_DIED);
    assert(observer.calls == 1);

    {
        auto scoped = bus.subscribe(EventType::PLAYER_DIED, &observer);
        assert(scoped.connected());
        bus.notify(EventType::PLAYER_DIED);
        assert(observer.calls == 2);
    }
    bus.notify(EventType::PLAYER_DIED);
    assert(observer.calls == 2);

    std::cout << "[PASSED] testScopeMoveAndReset\n";
}

void testDuplicateLeasesDisconnectOnLastToken() {
    EventBus& bus = EventBus::getInstance();
    CountingObserver observer;

    Subscription first = bus.subscribe(EventType::GAME_PAUSED, &observer);
    Subscription second = bus.subscribe(EventType::GAME_PAUSED, &observer);
    assert(first.connected());
    assert(second.connected());

    bus.notify(EventType::GAME_PAUSED);
    assert(observer.calls == 1);

    first.reset();
    assert(!first.connected());
    assert(second.connected());
    bus.notify(EventType::GAME_PAUSED);
    assert(observer.calls == 2);

    second.reset();
    assert(!second.connected());
    bus.notify(EventType::GAME_PAUSED);
    assert(observer.calls == 2);

    std::cout << "[PASSED] testDuplicateLeasesDisconnectOnLastToken\n";
}

void testRemovalAndResetDuringDispatch() {
    EventBus& bus = EventBus::getInstance();
    CountingObserver target;
    Subscription targetToken = bus.subscribe(EventType::LEVEL_STARTED, &target);
    RemovingObserver remover(bus, targetToken);
    Subscription removerToken = bus.subscribe(EventType::LEVEL_STARTED, &remover);

    // Move the remover ahead of target in deterministic subscription order by
    // disconnecting and re-registering both observers.
    targetToken.reset();
    removerToken.reset();
    targetToken = bus.subscribe(EventType::LEVEL_STARTED, &target);
    removerToken = bus.subscribe(EventType::LEVEL_STARTED, &remover);
    // The target is now first, so use a fresh event to place remover first.
    targetToken.reset();
    removerToken.reset();
    removerToken = bus.subscribe(EventType::LEVEL_STARTED, &remover);
    targetToken = bus.subscribe(EventType::LEVEL_STARTED, &target);

    bus.notify(EventType::LEVEL_STARTED);
    assert(remover.calls == 1);
    assert(target.calls == 0);
    assert(!targetToken.connected());

    removerToken.reset();
    std::cout << "[PASSED] testRemovalAndResetDuringDispatch\n";
}

void testReentrantDispatch() {
    EventBus& bus = EventBus::getInstance();
    CountingObserver target;
    Subscription targetToken = bus.subscribe(EventType::GAME_RESUMED, &target);
    RemovingObserver reentrant(bus, targetToken);
    reentrant.reenter = true;
    Subscription reentrantToken = bus.subscribe(EventType::GAME_RESUMED, &reentrant);

    // Place the reentrant observer first; it triggers one nested dispatch and
    // then resets target before the outer snapshot reaches it.
    targetToken.reset();
    reentrantToken.reset();
    reentrantToken = bus.subscribe(EventType::GAME_RESUMED, &reentrant);
    targetToken = bus.subscribe(EventType::GAME_RESUMED, &target);

    bus.notify(EventType::GAME_RESUMED);
    assert(reentrant.calls == 2);
    assert(target.calls == 0);

    reentrantToken.reset();
    std::cout << "[PASSED] testReentrantDispatch\n";
}

void testValueContextDelivery() {
    EventBus& bus = EventBus::getInstance();
    CountingObserver observer;
    Subscription token = bus.subscribe(EventType::LEVEL_STARTED, &observer);

    bus.notify(GameEvent{EventType::LEVEL_STARTED, LevelContext{7}});
    assert(observer.calls == 1);
    assert(observer.lastEvent.type == EventType::LEVEL_STARTED);
    const auto* level = std::get_if<LevelContext>(&observer.lastEvent.context);
    assert(level != nullptr);
    assert(level->level == 7);

    token.reset();
    std::cout << "[PASSED] testValueContextDelivery\n";
}

void testLifecycleCycles() {
    EventBus& bus = EventBus::getInstance();
    int calls = 0;

    struct CycleObserver final : IObserver {
        explicit CycleObserver(int& count) : calls(count) {}
        void onNotify(const GameEvent&) override { ++calls; }
        int& calls;
    };

    for (int i = 0; i < 200; ++i) {
        CycleObserver observer(calls);
        auto token = bus.subscribe(EventType::COIN_COLLECTED, &observer);
        bus.notify(EventType::COIN_COLLECTED);
        assert(token.connected());
    }

    bus.notify(EventType::COIN_COLLECTED);
    assert(calls == 200);
    std::cout << "[PASSED] testLifecycleCycles\n";
}

} // namespace

int main() {
    testMoveOnlyTraits();
    testTemporaryTokenDisconnectsImmediately();
    testScopeMoveAndReset();
    testDuplicateLeasesDisconnectOnLastToken();
    testRemovalAndResetDuringDispatch();
    testReentrantDispatch();
    testValueContextDelivery();
    testLifecycleCycles();
    std::cout << "All EventBus tests passed successfully!\n";
    return 0;
}
