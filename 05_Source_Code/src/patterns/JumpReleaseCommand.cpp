/**
 * @file JumpReleaseCommand.cpp
 * @author TV5 (Truyen)
 * @brief Callback-based jump-release command.
 */

#include "patterns/JumpReleaseCommand.h"

JumpReleaseCommand::JumpReleaseCommand(Request request)
    : m_request(std::move(request)) {}

void JumpReleaseCommand::execute() {
    if (m_request) {
        m_request();
    }
}
