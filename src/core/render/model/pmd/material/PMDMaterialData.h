#pragma once
#include "../../../shader/SamplerStateNames.h"
#include "../../../texrure/Texture.h"
#include "SphereMode.h"
#include <optional>
#include <wrl/client.h>

struct ID3D11Buffer;
struct PMDMaterial;
struct ID3D11Device;

struct PMDMaterialData {
public:
    const Texture texture;
    const Texture sphere;
    const Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    const SphereMode sphere_mode;
    const uint32_t index_count;
    const uint8_t toon_index;
    const uint8_t edge_flag;

    static std::optional<PMDMaterialData> make(
        const PMDMaterial& material,
        ID3D11Device* const device,
        const std::filesystem::path& path
    );

private:
    struct Textures {
        SphereMode sphere_mode;
        Texture texture;
        Texture sphere;
    };

    static std::optional<Textures> load_texture(
        const PMDMaterial& material,
        ID3D11Device* const device,
        const std::filesystem::path& path
    );
    static std::optional<Microsoft::WRL::ComPtr<ID3D11Buffer>> make_buffer(
        const PMDMaterial& material,
        const Textures textures,
        ID3D11Device* const device
    );
};
