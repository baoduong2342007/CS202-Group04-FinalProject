/**
 * @file Subscription.h
 * @brief Move-only RAII token for an EventBus registration.
 */

#pragma once

#include <memory>

namespace eventbus_detail {
struct EventBusState;
struct SubscriptionLease;
} // namespace eventbus_detail

/**
 * Owns one reference to a logical EventBus registration.
 *
 * A token is intentionally move-only.  Destroying or resetting the last token
 * for an observer/event pair disconnects that registration.  The connection
 * lease itself only weakly refers to EventBus state, so a token can safely
 * outlive the singleton during static teardown.
 */
class Subscription final {
public:
    Subscription() noexcept = default;
    ~Subscription() noexcept;

    Subscription(const Subscription&) = delete;
    Subscription& operator=(const Subscription&) = delete;

    Subscription(Subscription&& other) noexcept;
    Subscription& operator=(Subscription&& other) noexcept;

    /// Disconnect this token.  Calling reset repeatedly is harmless.
    void reset() noexcept;

    /// True while this token still belongs to a live logical registration.
    bool connected() const noexcept;

    explicit operator bool() const noexcept { return connected(); }

private:
    friend class EventBus;

    explicit Subscription(
        std::shared_ptr<eventbus_detail::SubscriptionLease> lease) noexcept;

    std::shared_ptr<eventbus_detail::SubscriptionLease> m_lease;
};
