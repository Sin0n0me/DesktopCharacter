#pragma once
#include "../errors/renderer_errors.h"
#include "renderer.h"
#include <engine_types/renderer/render_handle.h>
#include <engine_types/window/window_handle.h>
#include <foundation/result/result.h>
#include <optional>

namespace enishi::platform {
    // 各プラットフォームに応じたウィンドウハンドルを受け取って初期化する実装することで
    // IRendererを返すことでIRendererはプラットフォームに応じたウィンドウを知らずに初期化ができる
    class IRendererInitializer {
      public:
        virtual ~IRendererInitializer(void) noexcept = default;

        //virtual foundation::EngineResult<IRenderer*, RenderError> get(void) const noexcept = 0;

               virtual std::optional<IRenderer*> set(void) const noexcept = 0;
        virtual std::optional<IRenderer*> get(void) const noexcept = 0;
    };
} // namespace enishi::platform