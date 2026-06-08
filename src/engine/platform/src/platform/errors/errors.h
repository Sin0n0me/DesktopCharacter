#pragma once
#include <string>

namespace platform {
    namespace error {
        enum class _Error {};

        namespace {
            enum class ErrorTag {
                _Error,
            };

            union ErrorKind {
                _Error _error;
            };
        } // namespace

        struct Error {
            std::u8string message;
            ErrorTag error_tag;
            ErrorKind error;

          public:
            ~Error(void) noexcept = default;
        };
    } // namespace error
} // namespace platform
