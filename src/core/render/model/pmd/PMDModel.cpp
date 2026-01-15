#include "../../../utility/Algorithm.h"
#include "../../../utility/BinaryReader.h"
#include "../../../utility/Maker.h"
#include "../../constant_buffer/ConstantBufferNames.h"
#include "../../constant_buffer/Material.h"
#include "../../motion/vmd/VMDMotion.h"
#include "../../shader/Shader.h"
#include "../../shader/ShaderBindingSlots.h"
#include "morph/PMDMorphManager.h"
#include "PMDModel.h"
#include "PMDModelLoader.h"
#include <d3d11.h>

constexpr float WEIGHT_THRESHOLD = 0.3f;

PMDModel::PMDModel(const std::filesystem::path& path) : Model(path) {
    Maker::make_shared(this->model_loader, path);
    Maker::make_shared(this->vertices);
}

Model::Model(const std::filesystem::path& path) : path(path) {}

void PMDModel::on_clicked(const short obb_index) {
}

bool PMDModel::init(ID3D11Device* const device) {
    if(this->is_loaded_model()) {
        return true;
    }

    // pmdの読み込み
    if(!this->model_loader->load_pmd()) {
        return false;
    }

    // バッファ作成
    if(!this->make_buffers(device)) {
        return false;
    }

    // ボーン名の解決
    Maker::make_shared(
        this->bone_manager,
        this->model_loader->get_bones(),
        this->model_loader->get_iks()
    );
    if(!this->bone_manager->init(device)) {
        return false;
    }

    // モーフ
    Maker::make_shared(
        this->morph_manager,
        this->model_loader->get_morphs(),
        this->vertices
    );
    if(!this->morph_manager->init(device)) {
        return false;
    }

    // モーション
    Maker::make_shared(
        this->motion_manager,
        this->bone_manager
    );
    if(!this->motion_manager->init()) {
        return false;
    }

    return true;
}

void PMDModel::render_update(ID3D11DeviceContext* const context) {
    this->bone_manager->render_update(context);
    this->morph_manager->render_update(context);

    D3D11_MAPPED_SUBRESOURCE mapped;
    context->Map(this->vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, this->vertices.get(), sizeof(decltype(this->vertices)::element_type));
    context->Unmap(this->vertex_buffer.Get(), 0);
}

void PMDModel::render(
    ID3D11DeviceContext* const context,
    const ShaderBindingSlots* slots
) const {
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    const UINT stride = sizeof(PMDVertexData);
    const UINT offset = 0;
    context->IASetVertexBuffers(
        0,
        1,
        this->vertex_buffer.GetAddressOf(),
        &stride,
        &offset
    );
    context->IASetIndexBuffer(
        this->index_buffer.Get(),
        DXGI_FORMAT_R16_UINT,
        0
    );

    this->bone_manager->render(context, slots);

    uint32_t index_offset = 0;
    for(const auto& material : this->materials) {
        material.texture.render_set_resource(context, slots);
        material.sphere.render_set_resource(context, slots);

        // シャドウマップなどで使用しない場合もある
        const BindingSlotKey key = BindingSlotKey{
            static_cast<uint32_t>(ConstantBufferName::Material),
            ShaderType::Pixel,
            BindingSlotKind::ConstantBuffer,
        };
        if(slots->contains(key)) {
            context->PSSetConstantBuffers(
                slots->get(key),
                1,
                material.buffer.GetAddressOf()
            );
        }

        context->DrawIndexed(material.index_count, index_offset, 0);
        index_offset += material.index_count;
    }
}

void PMDModel::unload_model() {
}

bool PMDModel::is_loaded_model() {
    return bool(this->bone_manager)
        && bool(this->motion_manager)
        && bool(this->morph_manager);
}

// 主成分分析による最小体積のOBBを作成
void PMDModel::compute_obb(std::unordered_map<short, OBB>& obb_map) {
    // ボーンが影響する頂点のみを収集
    std::unordered_map<short, std::vector<DirectX::XMFLOAT3>> points_map;
    for(const auto& vertex : this->model_loader->get_vertices()) {
        if(vertex.bone_weight > WEIGHT_THRESHOLD) {
            points_map[vertex.bone_index[0]].emplace_back(vertex.position);
        }
    }

    // ボーンのローカル空間へ変換と共分散行列の作成
    for(auto& [index, positions] : points_map) {
        obb_map.emplace(index, OBB::make_by_covariance_matrix(positions));
    }
}

void PMDModel::update_obb(std::unordered_map<short, OBB>& obb_map) {
}

void PMDModel::update_motion(const int64_t delta_time) {
    this->motion_manager->update_motion(delta_time);
}

bool PMDModel::make_buffers(ID3D11Device* const device) {
    // 頂点バッファ作成
    if(!this->make_vertex_buffer(device)) {
        return false;
    }

    // インデックスバッファ作成
    if(!this->make_index_buffer(device)) {
        return false;
    }

    // 定数バッファ作成
    if(!this->make_constant_buffer(device)) {
        return false;
    }

    return true;
}

bool PMDModel::make_vertex_buffer(ID3D11Device* const device) {
    const auto& loaded_vertices = this->model_loader->get_vertices();
    const auto size = loaded_vertices.size();
    this->vertices->resize(size);
    for(size_t i = 0; i < size; ++i) {
        auto& dst_vertex = this->vertices->at(i);
        auto& src_vertex = loaded_vertices.at(i);

        memcpy(
            dst_vertex.position,
            src_vertex.position,
            sizeof(decltype(dst_vertex.position))
        );
        memcpy(
            dst_vertex.normal,
            src_vertex.normal,
            sizeof(decltype(dst_vertex.normal))
        );
        memcpy(
            dst_vertex.uv,
            src_vertex.uv,
            sizeof(decltype(dst_vertex.uv))
        );
        memcpy(
            dst_vertex.bone_index,
            src_vertex.bone_index,
            sizeof(decltype(dst_vertex.bone_index))
        );

        // 正規化
        const float weight = static_cast<float>(src_vertex.bone_weight) / 100.0f;
        const float bone_weight[2] = {
            weight,
            1.0f - weight
        };
        memcpy(
            dst_vertex.bone_weight,
            bone_weight,
            sizeof(decltype(dst_vertex.bone_weight))
        );
    }

    const D3D11_BUFFER_DESC desc{
        .ByteWidth = static_cast<UINT>(sizeof(PMDVertexData) * this->vertices->size()),
        .Usage = D3D11_USAGE_DYNAMIC,
        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    };
    const D3D11_SUBRESOURCE_DATA init_data{
        .pSysMem = this->vertices->data(),
    };

    const HRESULT hr = device->CreateBuffer(
        &desc,
        &init_data,
        this->vertex_buffer.GetAddressOf()
    );
    if FAILED(hr) {
        return false;
    }

    return true;
}

bool PMDModel::make_index_buffer(ID3D11Device* const device) {
    const auto& indices = this->model_loader->get_indices();
    const D3D11_BUFFER_DESC desc{
        .ByteWidth = UINT(sizeof(uint16_t) * indices.size()),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_INDEX_BUFFER,
    };
    const D3D11_SUBRESOURCE_DATA init_data = {
        .pSysMem = indices.data()
    };

    const HRESULT hr = device->CreateBuffer(
        &desc,
        &init_data,
        this->index_buffer.GetAddressOf()
    );
    if FAILED(hr) {
        return false;
    }

    return true;
}

bool PMDModel::make_constant_buffer(ID3D11Device* const device) {
    for(const auto& material : this->model_loader->get_materials()) {
        PMDMaterialData material_data{
            .sphere_mode = SphereMode::None,
            .index_count = material.index_count,
        };

        // スフィアがついている場合があるので分離
        std::string	texture_path = material.texture_file;
        const auto pos = texture_path.find('*');
        if(pos == std::string::npos) {
            // スフィアがない場合はダミーを作成
            if(!material_data.sphere.make_dummy_texture(device)) {
                return false;
            }
        } else {
            std::string sphere = texture_path.substr(pos + 1);
            texture_path = texture_path.substr(0, pos);

            if(sphere.ends_with(".sph")) {
                material_data.sphere_mode = SphereMode::Multiply;
            } else if(sphere.ends_with(".spa")) {
                material_data.sphere_mode = SphereMode::Add;
            } else {
                return false;
            }

            if(!material_data.sphere.load_texure(
                device,
                this->path.parent_path().append(sphere)
            )) {
                return false;
            }
        }

        if(!material_data.texture.load_texure(
            device,
            this->path.parent_path().append(texture_path)
        )) {
            return false;
        }

        const D3D11_BUFFER_DESC desc{
            .ByteWidth = sizeof(Material),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
        };
        Material mat{
            .sphere_mul = 0.0f,
            .sphere_add = 0.0f,
        };
        memcpy(mat.diffuse, material.diffuse, sizeof(float) * 4);
        memcpy(mat.ambient, material.ambient, sizeof(float) * 3);

        switch(material_data.sphere_mode) {
        case SphereMode::None:
            break;
        case SphereMode::Multiply:
            mat.sphere_mul = 1.0f;
            break;
        case SphereMode::Add:
            mat.sphere_add = 1.0f;
            break;
        default:
            return false;
        }

        const D3D11_SUBRESOURCE_DATA init_data{
            .pSysMem = &mat,
            .SysMemPitch = 0,
            .SysMemSlicePitch = sizeof(Material),
        };

        const HRESULT hr = device->CreateBuffer(
            &desc,
            &init_data,
            material_data.buffer.GetAddressOf()
        );
        if FAILED(hr) {
            return false;
        }

        this->materials.emplace_back(material_data);
    }

    return true;
}