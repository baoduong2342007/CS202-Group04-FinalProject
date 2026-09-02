/**
 * @file GameEvent.h
 * @brief Value-based event and context data passed through EventBus.
 */

#pragma once

#include <utility>
#include <variant>

#include "patterns/EventType.h"

/** A context marker for events which do not carry additional data. */
struct NoEventContext {
};

/** Value-only player context.  The ID is deliberately opaque to EventBus. */
struct PlayerEventContext {
    int playerId = 0;
};

/** Value-only level context. */
struct LevelEventContext {
    int level = 0;
};

/** Value-only score/amount context. */
struct ScoreEventContext {
    int amount = 0;
};

// Short aliases keep call sites readable while retaining explicit event names
// for users that prefer the more descriptive forms above.
using NoContext = NoEventContext;
using PlayerContext = PlayerEventContext;
using LevelContext = LevelEventContext;
using ScoreContext = ScoreEventContext;

/**
 * Closed set of value-only contexts supported by the event system.
 *
 * Keep pointers, references, type-erased payloads, and domain-owned objects
 * out of this variant.  EventBus may retain/copy a GameEvent while it is
 * dispatching.
 */
using EventContext = std::variant<NoEventContext,
                                  PlayerEventContext,
                                  LevelEventContext,
                                  ScoreEventContext>;

/** A globally published event and its optional value context. */
struct GameEvent {
    EventType type;
    EventContext context;

    explicit GameEvent(EventType event)
        : type(event), context(NoEventContext{}) {}

    GameEvent(EventType event, EventContext eventContext)
        : type(event), context(std::move(eventContext)) {}
};
