#pragma once
#include "errors/errors.h"
#include <engine_types/assets/asset_handle.h>
#include <foundation/result/result.h>

namespace enishi::assets_system {
    using AssetResult = foundation::EngineResult<types::AssetHandle, IOError>;
} // namespace enishi::assets_system
