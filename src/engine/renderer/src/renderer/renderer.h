#pragma once

/**
 * この層では描画に関する責務を負う
 *
 */

#include <vector>
#include <glm/glm.hpp>
#include "render_handle.h"

namespace enishi::renderer {

    struct DrawCommand {
        RenderHandle mesh;
        RenderHandle material;
        glm::mat4 transform;
    };

    enum class RenderPassType {
        Shadow,
        Model,
        Lighting,
        Transparent,
        PostProcess,
        UI
    };

    struct RenderPass {
        RenderPassType type;
        RenderHandle render_target; // NullHandle でスワップチェーンに描く
        std::vector<DrawCommand> commands;
    };

} // namespace renderer