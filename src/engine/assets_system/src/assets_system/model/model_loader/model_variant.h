#pragma once
#include "pmd/pmd_data.h"
#include <variant>

namespace enishi::assets_system {
    using ModelData = std::variant<std::monostate, std::unique_ptr<PMDData>>;
} // namespace enishi::assets_system