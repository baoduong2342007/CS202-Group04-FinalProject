/**
 * @file TimeUtils.h
 * @author Group 04
 * @brief Shared date/time formatting utility used by game record persistence.
 */

#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace TimeUtils {

/**
 * @brief Returns the current local date and time as "YYYY-MM-DD HH:MM".
 *
 * Used by PlayState, GameOverState, and PvpPlayState when creating
 * GameRecord entries for the match history log.
 */
inline std::string getCurrentDateTimeString() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &now_c);
#else
    localtime_r(&now_c, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M");
    return oss.str();
}

} // namespace TimeUtils
