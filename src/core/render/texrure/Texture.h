#pragma once
#include <d3d11.h>
#include <filesystem>
#include <unordered_map>
#include <wrl/client.h>

class ShaderBindingSlots;

class Texture {
private:
    struct TextureResouce {
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resouce_view;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
    };

    static std::unordered_map<uint32_t, TextureResouce> texture_map;
    static TextureResouce dummy_texture;
    uint32_t file_path_hash;
    bool use_dummy_texture;

private:

    bool make_texture(
        ID3D11Device* const device,
        ID3D11Texture2D** texture,
        const UINT width,
        const UINT height,
        const D3D11_SUBRESOURCE_DATA& data
    );

    bool make_sampler(ID3D11Device* const device);

public:
    const uint32_t texture_name_hash;
    const uint32_t sampler_name_hash;

public:
    explicit Texture(
        const uint32_t texture_name_hash,
        const uint32_t sampler_name_hash
    ) noexcept;

    bool load_texure(
        ID3D11Device* const device,
        const std::filesystem::path path
    );

    bool make_dummy_texture(ID3D11Device* const device);

    void set_resource(
        ID3D11DeviceContext* const context,
        const ShaderBindingSlots* slots
    ) const;

    static void set_dummy_resouce(
        ID3D11DeviceContext* const context,
        const ShaderBindingSlots* slots,
        const uint32_t texture_name_hash,
        const uint32_t sampler_name_hash
    );
};
