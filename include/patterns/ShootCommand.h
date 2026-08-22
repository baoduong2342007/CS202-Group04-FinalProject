/**
 * @file ShootCommand.h
 * @author TV5 (Truyen)
 * @brief Command that requests a projectile shot.
 *
 * Input only emits the request. Projectile ownership and creation remain in
 * the gameplay/physics module, so InputHandler never constructs a FireBall.
 */

#pragma once

#include <functional>

#include "patterns/ICommand.h"

class ShootCommand final : public ICommand {
public:
    using Request = std::function<void()>;

    explicit ShootCommand(Request request);

    void execute() override;

private:
    Request m_request;
};
