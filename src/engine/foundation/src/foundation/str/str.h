#pragma once
#include <cstdint>
#include <string>
#include <unicode/unistr.h>

namespace enishi ::foundation {
    using UTF8 = std::string;

    enum class StringError : std::uint32_t {
        EncodeError,
        DecodeError,
    };
} // namespace enishi::foundation