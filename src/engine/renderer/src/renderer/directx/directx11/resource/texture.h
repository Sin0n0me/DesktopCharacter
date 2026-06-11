#pragma once
#include <d3d11.h>
#include <wrl/client.h>

namespace enishi::renderer::directx {
    enum class TextureType {
        Texture2D,
    };

    struct Texture {
        TextureType texture_type;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    };
} // namespace enishi::renderer::directx