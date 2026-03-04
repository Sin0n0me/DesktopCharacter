#pragma once
#include "LogLevel.h"
#include <chrono>
#include <string>

struct LogEntry {
    LogLevel level;
    std::u8string message;
    std::chrono::system_clock::time_point time;

    std::u8string format_log(void) const;
};
