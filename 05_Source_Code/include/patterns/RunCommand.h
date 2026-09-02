/**
 * @file RunCommand.h
 * @author TV5 (Truyen)
 * @brief Command that requests the run state while its key is held.
 *
 * The command deliberately receives a callback instead of spawning or
 * mutating a gameplay object directly. The owner of the Mario/physics module
 * can provide the callback when the run-state consumer is ready.
 */

#pragma once

#include <functional>

#include "patterns/ICommand.h"

class RunCommand final : public ICommand {
public:
    using Request = std::function<void()>;

    explicit RunCommand(Request request);

    void execute() override;

private:
    Request m_request;
};
