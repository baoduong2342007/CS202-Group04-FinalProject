/**
 * @file RunCommand.cpp
 * @author TV5 (Truyen)
 * @brief Callback-based run request command.
 */

#include "patterns/RunCommand.h"

RunCommand::RunCommand(Request request)
    : m_request(std::move(request)) {}

void RunCommand::execute() {
    if (m_request) {
        m_request();
    }
}
