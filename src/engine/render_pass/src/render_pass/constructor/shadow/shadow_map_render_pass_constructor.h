#pragma once
#include <foundation/constexpr/hash/char_array_to_hash.h>
#include <render_pass/constructor/interface_render_pass_construstor.h>
#include <vector>

namespace enishi::render_pass {
    class ShadowMapRenderPassConstructor : public IRenderPassConstructor {
      public:
        static constexpr char RENDER_PASS_NAME[] = "ShadowMap";
        static constexpr foundation::DependencyNode NODE{foundation::hash_size_t(RENDER_PASS_NAME)};

      public:
        foundation::Result<std::shared_ptr<platform::IRenderPass>, ConstructError> make(
            platform::IRenderer* const renderer) override;

      private:
        foundation::DependencyNode get_node(void) const noexcept override;
        foundation::DependencyBounds get_dependencies(void) const noexcept override;
    };
} // namespace enishi::render_pass