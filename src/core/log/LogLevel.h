#pragma once
#include <string>

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

const std::u8string level_to_string(const LogLevel& level);
