#include "d3d11_render_initializer.h"

namespace enishi::renderer::directx {
    foundation::EngineResult<std::unique_ptr<D3D11Renderer>, platform::RenderError>
    D3D11RenderInitializer::init(
        const platform::NativeWindowHandle& native_handle, const types::WindowSize& window_size) {
        auto d3d11 = D3D11::make(native_handle.hwnd, window_size);
        if (d3d11.is_err()) {
            return d3d11.add_message(u8"Rendererの初期化に失敗しました")
                .propagation(platform::RenderError::MakeError)
                .error();
        }

        return std::make_unique<D3D11Renderer>(std::move(d3d11.value()));
    }
} // namespace enishi::renderer::directx