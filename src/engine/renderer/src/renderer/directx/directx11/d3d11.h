#pragma once
#include "../../errors/errors.h"
#include <d3d11.h>
#include <dcomp.h>
#include <engine_types/window/window_types.h>
#include <foundation/result/result.h>
#include <memory>
#include <wrl/client.h>

namespace enishi::renderer::directx {
    class D3D11 {
      private:
        Microsoft::WRL::ComPtr<IDXGIDevice> dxgi_device;
        Microsoft::WRL::ComPtr<IDXGIAdapter> dxgi_adapter;
        Microsoft::WRL::ComPtr<IDXGIFactory2> dxgi_factory;
        Microsoft::WRL::ComPtr<IDXGISwapChain1> dxgi_swap_chain;
        Microsoft::WRL::ComPtr<IDCompositionDevice> dcomp_device;
        Microsoft::WRL::ComPtr<IDCompositionTarget> dcomp_target;
        Microsoft::WRL::ComPtr<IDCompositionVisual> dcomp_visual;
        Microsoft::WRL::ComPtr<IDCompositionSurface> dcomp_surface;
        Microsoft::WRL::ComPtr<ID3D11Device> device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

      private:
        foundation::VoidResult<DirectXError> init(const HWND hwnd, const types::WindowSize& size);
        foundation::VoidResult<DirectXError> make_device(void);
        foundation::VoidResult<DirectXError> make_factory(void);
        foundation::VoidResult<DirectXError> make_surface(const types::WindowSize& size);
        foundation::VoidResult<DirectXError> make_swap_chain(const types::WindowSize& size);
        foundation::VoidResult<DirectXError> make_target(const HWND hwnd);
        foundation::VoidResult<DirectXError> make_visual(void);
        foundation::VoidResult<DirectXError> commit(void);

      public:
        static foundation::EngineResult<std::unique_ptr<D3D11>, DirectXError> make(
            const HWND hwnd, const types::WindowSize& size);

        [[nodiscard]] ID3D11Device* get_device(void) const;
        [[nodiscard]] ID3D11DeviceContext* get_context(void) const;
        [[nodiscard]] IDXGISwapChain1* get_swap_chain(void) const;
    };
} // namespace enishi::renderer::directx