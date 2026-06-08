#pragma once
#include "log_level.h"
#include <chrono>
#include <string>

namespace enishi ::foundation {
    struct LogEntry {
        LogLevel level;
        std::u8string message;
        std::chrono::system_clock::time_point time;

        std::u8string format_log(void) const;
    };
} // namespace enishi::foundation