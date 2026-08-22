/**
 * @file ShootCommand.cpp
 * @author TV5 (Truyen)
 * @brief Callback-based shoot request command.
 */

#include "patterns/ShootCommand.h"

ShootCommand::ShootCommand(Request request)
    : m_request(std::move(request)) {}

void ShootCommand::execute() {
    if (m_request) {
        m_request();
    }
}
