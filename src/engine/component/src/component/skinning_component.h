#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace enishi::component {
    // 各ボーンの最終的なスキニング行列(= global * bind_global_inverse)
    // GPUのボーン用バッファへ転送する直前のデータであり、SkinningSystemが毎フレーム書き込む
    struct SkinningComponent {
        std::vector<glm::mat4> skinning_matrices;
    };
} // namespace enishi::component
