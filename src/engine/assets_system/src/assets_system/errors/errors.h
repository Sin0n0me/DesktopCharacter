#pragma once
#include <foundation/errors/errors.h>
#include <string>

namespace enishi::assets_system {
    enum class IOError {
        FileNotFound,
        PermissionDenied,
        InvalidFormat,
        ReadFailed,
        UnexpectedEof,
        InvalidData,
        InvalidStreamPosition,
        BrokenStream,
        MismatchHeader,
    };

    template <typename T> using IOReuslt = foundation::Result<T, foundation::Error<IOError>>;
} // namespace enishi::assets_system