#pragma once
#include <foundation/result/result.h>
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

    enum class AssetError {
        AlreadyHasAsset,
        NotFound,
        IOError,
    };

    template <typename T> using IOReuslt = foundation::Result<T, IOError>;
} // namespace enishi::assets_system