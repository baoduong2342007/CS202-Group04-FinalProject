/**
 * @file JumpReleaseCommand.h
 * @author TV5 (Truyền)
 * @brief Command bridge for the jump-release frame.
 */

#pragma once

#include <functional>

#include "patterns/ICommand.h"

class JumpReleaseCommand final : public ICommand {
public:
    using Request = std::function<void()>;

    explicit JumpReleaseCommand(Request request);

    void execute() override;

private:
    Request m_request;
};
