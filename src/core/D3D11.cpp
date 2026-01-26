#include "../Application.h"
#include "Core.h"
#include "D3D11.h"
#include "log/Logger.h"
#include <d3d11.h>
#include <dcomp.h>
#include <dxgi.h>
#include <dxgi1_2.h>

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "dcomp.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "windowscodecs.lib")

//
// https://learn.microsoft.com/ja-jp/archive/msdn-magazine/2014/june/windows-with-c-high-performance-window-layering-using-the-windows-composition-engine
//

bool D3D11::init_d3d11(const HWND hwnd, const UINT width, const UINT height) {
    if(!this->make_device()) {
        return false;
    }

    if(!this->make_factory()) {
        return false;
    }

    if(!this->make_swap_chain(width, height)) {
        return false;
    }

    if(!this->make_target(hwnd)) {
        return false;
    }

    if(!this->make_visual()) {
        return false;
    }

    if(!this->commit()) {
        return false;
    }

    Logger::info(u8"D3D11の初期化に成功しました");

    return true;
}

// デバイスの作成
bool D3D11::make_device(void) {
    const HRESULT result_device = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT | (IS_DEBUG_MODE ? D3D11_CREATE_DEVICE_DEBUG : 0),
        nullptr,
        0,
        D3D11_SDK_VERSION,
        this->device.GetAddressOf(),
        nullptr,
        this->context.GetAddressOf()
    );
    if(FAILED(result_device)) {
        Logger::error(u8"D3D11デバイスの作成に失敗しました");
        return false;
    }

    const HRESULT result_dxgi_device = this->device.As(&this->dxgi_device);
    if(FAILED(result_device)) {
        Logger::error(u8"DXGIデバイスの作成に失敗しました");
        return false;
    }

    const HRESULT result_dcom_device = DCompositionCreateDevice(
        dxgi_device.Get(),
        __uuidof(this->dcomp_device),
        reinterpret_cast<void**>(this->dcomp_device.GetAddressOf())
    );
    if(FAILED(result_device)) {
        Logger::error(u8"DCompositionデバイスの作成に失敗しました");
        return false;
    }

    return true;
}

// DXGIFactoryの作成
bool D3D11::make_factory(void) {
    const HRESULT result_factory = CreateDXGIFactory2(
        DXGI_CREATE_FACTORY_DEBUG,
        __uuidof(this->dxgi_factory),
        reinterpret_cast<void**>(this->dxgi_factory.GetAddressOf())
    );
    if(FAILED(result_factory)) {
        Logger::error(u8"DXGIFactoryの作成に失敗しました");
        return false;
    }

    return true;
}

bool D3D11::make_surface(void) {
    const HRESULT result = this->dcomp_device->CreateSurface(
        WIDTH,
        HEIGHT,
        DXGI_FORMAT_B8G8R8A8_UNORM,
        DXGI_ALPHA_MODE_PREMULTIPLIED,
        this->dcomp_surface.GetAddressOf()
    );
    if(FAILED(result)) {
        Logger::error(u8"Surface(Composition)の作成に失敗しました");
        return false;
    }

    return true;
}

// スワップチェーンの作成
bool D3D11::make_swap_chain(const UINT width, const UINT height) {
    constexpr DXGI_SAMPLE_DESC sample{
        .Count = 1
    };
    const DXGI_SWAP_CHAIN_DESC1 description{
        .Width = width,
        .Height = height,
        .Format = DXGI_FORMAT_B8G8R8A8_UNORM,
        .SampleDesc = sample,
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 2,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
        .AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED,
    };
    const HRESULT result_swap_chain = this->dxgi_factory->CreateSwapChainForComposition(
        this->dxgi_device.Get(),
        &description,
        nullptr,
        this->dxgi_swap_chain.GetAddressOf()
    );

    if(FAILED(result_swap_chain)) {
        Logger::error(u8"SwapChain(Composition)の作成に失敗しました");
        return false;
    }

    return true;
}

bool D3D11::make_target(const HWND hwnd) {
    const HRESULT hr = this->dcomp_device->CreateTargetForHwnd(
        hwnd,
        true, // Top most
        this->dcomp_target.GetAddressOf()
    );

    if(FAILED(hr)) {
        Logger::error(u8"Target(Composition)の作成に失敗しました");
        return false;
    }

    return true;
}

bool D3D11::make_visual(void) {
    const HRESULT hr = this->dcomp_device->CreateVisual(this->dcomp_visual.GetAddressOf());
    if(FAILED(hr)) {
        Logger::error(u8"Visual(Composition)の作成に失敗しました");
        return false;
    }

    return true;
}

bool D3D11::commit(void) {
    const HRESULT result_set_context = this->dcomp_visual->SetContent(this->dxgi_swap_chain.Get());
    if(FAILED(result_set_context)) {
        Logger::error(u8"SwapChain(Composition)のセットに失敗しました");
        return false;
    }

    const HRESULT result_set_root = this->dcomp_target->SetRoot(this->dcomp_visual.Get());
    if(FAILED(result_set_root)) {
        Logger::error(u8"Visual(Composition)のセットに失敗しました");
        return false;
    }

    // 合成エンジンに完了を通知
    const HRESULT result_commit = this->dcomp_device->Commit();
    if(FAILED(result_commit)) {
        Logger::error(u8"Commitに失敗しました");
        return false;
    }

    return true;
}