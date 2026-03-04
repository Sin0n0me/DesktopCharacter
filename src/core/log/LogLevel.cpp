#include "LogLevel.h"

const std::u8string level_to_string(const LogLevel& level) {
    switch(level) {
    case LogLevel::Debug:
        return u8"Debug";
    case LogLevel::Info:
        return u8"Info";
    case LogLevel::Warning:
        return u8"Warning";
    case LogLevel::Error:
        return u8"Error";
    }
    return u8"Unknown";
}