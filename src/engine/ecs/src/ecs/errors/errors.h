#pragma once
#include <foundation/errors/errors.h>

namespace enishi::ecs {
    enum class ECSError {
        AlreadyHasComponent,
    };

    template <typename T> using ECSReuslt = foundation::Result<T, foundation::Error<ECSError>>;
} // namespace enishi::ecs