#pragma once
#include "render_data.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

namespace enishi::types {
    struct MeshData {
        std::weak_ptr<const RenderData> vertices;
        std::weak_ptr<const RenderData> indices;
    };
} // namespace enishi::types
