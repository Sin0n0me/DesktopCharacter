#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <platform/ik/interface_ik_bone_view.h>
#include <vector>

namespace enishi::skinning_system {
    class IKBoneView final : public platform::IIKBoneView {
      private:
        glm::quat* const rotation;
        glm::mat4* const global;
        std::vector<glm::mat4>* const globals;

      public:
        IKBoneView(glm::quat& rotation, glm::mat4& global, std::vector<glm::mat4>& globals) noexcept
            : rotation(&rotation)
            , global(&global)
            , globals(&globals) {
        }
    };
} // namespace enishi::skinning_system