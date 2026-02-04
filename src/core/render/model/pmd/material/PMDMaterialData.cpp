#include "../../../CommonResource.h"
#include "../../../constant_buffer/Material.h"
#include "../../../texrure/TextureNames.h"
#include "../PMDFileStruct.h"
#include "PMDMaterialData.h"
#include <filesystem>
#include <string>

std::optional<PMDMaterialData> PMDMaterialData::make(
    const PMDMaterial& material,
    ID3D11Device* const device,
    const std::filesystem::path& path
) {
    const auto opt_textures = PMDMaterialData::load_texture(
        material,
        device,
        path
    );
    if(!opt_textures.has_value()) {
        return std::nullopt;
    }
    const auto& textures = opt_textures.value();

    const auto opt_buffer = PMDMaterialData::make_buffer(
        material,
        textures,
        device
    );
    if(!opt_buffer.has_value()) {
        return std::nullopt;
    }
    const auto& buffer = opt_buffer.value();

    return PMDMaterialData{
        .texture = textures.texture,
        .sphere = textures.sphere,
        .buffer = buffer,
        .sphere_mode = textures.sphere_mode,
        .index_count = material.index_count,
        .toon_index = material.toon_index,
        .edge_flag = material.edge_flag,
    };
}

std::optional<PMDMaterialData::Textures> PMDMaterialData::load_texture(
    const PMDMaterial& material,
    ID3D11Device* const device,
    const std::filesystem::path& path
) {
    PMDMaterialData::Textures textures{
        .sphere_mode = SphereMode::None,
        .texture = Texture{
            static_cast<uint32_t>(TextureName::Model),
            static_cast<uint32_t>(SamplerStateName::Model)
    },
        .sphere = Texture{
            static_cast<uint32_t>(TextureName::Sphere),
            static_cast<uint32_t>(SamplerStateName::Sphere)
    }
    };

    // スフィアがついている場合があるので分離
    std::string	texture_path = material.texture_file;
    const auto pos = texture_path.find('*');
    if(pos == std::string::npos) {
        // スフィアがない場合はダミーを作成
        if(!textures.sphere.make_dummy_texture(device)) {
            return std::nullopt;
        }
    } else {
        std::string sphere = texture_path.substr(pos + 1);
        texture_path = texture_path.substr(0, pos);

        if(sphere.ends_with(".sph")) {
            textures.sphere_mode = SphereMode::Multiply;
        } else if(sphere.ends_with(".spa")) {
            textures.sphere_mode = SphereMode::Add;
        } else {
            return std::nullopt;
        }

        if(!textures.sphere.load_texure(
            device,
            path.parent_path() / sphere
        )) {
            return std::nullopt;
        }
    }

    if(!textures.texture.load_texure(
        device,
        path.parent_path() / texture_path
    )) {
        return std::nullopt;
    }

    return textures;
}

std::optional<Microsoft::WRL::ComPtr<ID3D11Buffer>> PMDMaterialData::make_buffer(
    const PMDMaterial& material,
    const Textures textures,
    ID3D11Device* const device
) {
    const D3D11_BUFFER_DESC desc{
        .ByteWidth = sizeof(Material),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
    };
    Material mat{
        .shininess = material.shininess,
        .sphere_mul = textures.sphere_mode == SphereMode::Multiply ? 1.0f : 0.0f,
        .sphere_add = textures.sphere_mode == SphereMode::Add ? 1.0f : 0.0f,
    };
    memcpy(mat.diffuse, material.diffuse, sizeof(material.diffuse));
    memcpy(mat.ambient, material.ambient, sizeof(material.ambient));
    memcpy(mat.specular, material.specular, sizeof(material.specular));

    const D3D11_SUBRESOURCE_DATA init_data{
        .pSysMem = &mat,
        .SysMemPitch = 0,
        .SysMemSlicePitch = sizeof(Material),
    };

    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    const HRESULT hr = device->CreateBuffer(
        &desc,
        &init_data,
        buffer.GetAddressOf()
    );
    if FAILED(hr) {
        return std::nullopt;
    }

    return buffer;
}