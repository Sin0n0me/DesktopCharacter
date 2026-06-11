#include "sdl3_render_initializer.h"

namespace enishi::platform_impl {
    foundation::EngineResult<platform::IRenderer*, platform::RenderError>
    enishi::platform_impl::SDL3RendererInitializer::get(void) const noexcept {
        return {};
    }
} // namespace enishi::platform_impl