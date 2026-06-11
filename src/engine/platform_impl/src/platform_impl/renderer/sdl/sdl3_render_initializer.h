#pragma once
#include <platform/renderer/render_initializer.h>

namespace enishi::platform_impl {
    class SDL3RendererInitializer : public platform::IRendererInitializer {
      public:
        foundation::EngineResult<platform::IRenderer*, platform::RenderError> get(
            void) const noexcept override;
    };
} // namespace enishi::platform_impl