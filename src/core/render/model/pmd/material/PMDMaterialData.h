#pragma once
#include "../../../shader/SamplerStateNames.h"
#include "../../../texrure/Texture.h"
#include "../../../texrure/TextureNames.h"
#include "SphereMode.h"
#include <wrl/client.h>

struct ID3D11Buffer;

struct PMDMaterialData {
    Texture texture = Texture(
        static_cast<uint32_t>(TextureName::Model),
        static_cast<uint32_t>(SamplerStateName::Model)
    );
    Texture sphere = Texture(
        static_cast<uint32_t>(TextureName::Sphere),
        static_cast<uint32_t>(SamplerStateName::Sphere)
    );
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    SphereMode sphere_mode;
    uint32_t index_count;
};
