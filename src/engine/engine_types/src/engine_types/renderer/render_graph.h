#pragma once
#include "camera.h"
#include "render_handle.h"
#include <glm/glm.hpp>

namespace enishi::types {
    enum class RenderCommand {
        Bind,
    };

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
        UI,
    };

    struct RenderPass {
        RenderPassType pass_type;
        RenderHandle render_target; // NullHandle でスワップチェーンに描く
        std::vector<DrawCommand> commands;
    };

    // フレーム全体の描画記述
    struct RenderGraph {
        std::vector<RenderPass> passes; // 順番通りに実行される
        Camera camera;                  // 全パス共通のカメラ情報
    };
} // namespace enishi::types