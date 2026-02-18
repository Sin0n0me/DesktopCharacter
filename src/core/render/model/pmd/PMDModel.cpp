#include "../../../log/Logger.h"
#include "../../../physics/mmd/joint/MMDJoint.h"
#include "../../../physics/mmd/MMDPhysics.h"
#include "../../../physics/mmd/MMDPhysicsWorld.h"
#include "../../../physics/mmd/motion_state/MMDMotionState.h"
#include "../../../physics/mmd/rigid_body/MMDRigidBody.h"
#include "../../constant_buffer/ConstantBufferNames.h"
#include "../../motion/vmd/VMDMotion.h"
#include "../../motion/vmd/VMDMotionManager.h"
#include "../../shader/Shader.h"
#include "../../shader/ShaderBindingSlots.h"
#include "../../texrure/TextureNames.h"
#include "bone/PMDBoneManager.h"
#include "ik/IKSolver.h"
#include "morph/PMDMorphManager.h"
#include "PMDModel.h"
#include "PMDModelLoader.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <d3d11.h>

constexpr float WEIGHT_THRESHOLD = 0.3f;
constexpr float BLEND_COLOR[4] = {0.0f, 0.0f, 0.0f, 0.0f};

PMDModel::PMDModel(const std::filesystem::path& path) :
    Model(path),
    model_loader(new PMDModelLoader(path)),
    vertices(new std::vector<PMDVertexData>()) {
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
    this->bone_manager = std::make_unique<PMDBoneManager>(
        this->model_loader->get_bones()
    );
    if(!this->bone_manager->init(device)) {
        return false;
    }

    // 物理エンジンの初期化
    this->physics = std::make_unique<MMDPhysics>(
        std::make_shared<MMDPhysicsWorld>(
            this->bone_manager->get_root_bones()
        )
    );
    if(!this->physics->init()) {
        return false;
    }

    // IK
    this->ik_soulver = std::make_unique<IKSolver>(
        this->bone_manager,
        this->model_loader->get_iks()
    );

    // モーフ
    this->morph_manager = std::make_unique<PMDMorphManager>(
        this->model_loader->get_morphs(),
        this->vertices
    );

    // モーション
    this->motion_manager = std::make_unique<VMDMotionManager>(
        this->bone_manager,
        this->morph_manager,
        this->ik_soulver,
        this->physics
    );
    if(!this->motion_manager->init()) {
        return false;
    }

    if(!this->make_blend_state(device)) {
        return false;
    }

    // 剛体
    if(!this->make_rigid_body()) {
        return false;
    }

    if(!this->make_joint()) {
        return false;
    }
    this->physics->reset_physics();

    return true;
}

void PMDModel::render_update(ID3D11DeviceContext* const context) {
    this->bone_manager->render_update(context);

    D3D11_MAPPED_SUBRESOURCE mapped;
    context->Map(
        this->vertex_buffer.Get(),
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mapped
    );
    memcpy(
        mapped.pData,
        this->vertices->data(),
        sizeof(PMDVertexData) * this->vertices->size()
    );
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

    context->OMSetBlendState(
        this->blend_state.Get(),
        BLEND_COLOR,
        0xFFFFFFFF
    );

    this->bone_manager->render(context, slots);

    uint32_t index_offset = 0;
    for(const auto& material : this->materials) {
        material.texture.set_resource(context, slots);
        material.sphere.set_resource(context, slots);

        if(material.toon_index < this->toon_textures.size()) {
            this->toon_textures
                .at(material.toon_index)
                .set_resource(context, slots);
        } else {
            Texture::set_dummy_resouce(
                context,
                slots,
                static_cast<uint32_t>(TextureName::Toon),
                static_cast<uint32_t>(SamplerStateName::Toon)
            );
        }

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
        && bool(this->morph_manager)
        && bool(this->ik_soulver)
        && bool(this->physics);
}

// 主成分分析による最小体積のOBBを作成
void PMDModel::compute_obb(std::unordered_map<short, OBB>& obb_map) {
    // ボーンが影響する頂点のみを収集
    std::unordered_map<short, std::vector<DirectX::XMFLOAT3>> points_map;
    for(const auto& vertex : this->model_loader->get_vertices()->vertices) {
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

void PMDModel::on_clicked(const short obb_index) {
}

void PMDModel::update_motion(const DeltaTime& delta_time) {
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
    if(!this->make_material_buffer(device)) {
        return false;
    }

    return true;
}

bool PMDModel::make_vertex_buffer(ID3D11Device* const device) {
    const auto& loaded_vertices = this->model_loader->get_vertices();
    const auto size = loaded_vertices->size;
    this->vertices->resize(size);
    for(size_t i = 0; i < size; ++i) {
        auto& dst_vertex = this->vertices->at(i);
        auto& src_vertex = loaded_vertices->vertices.at(i);

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

        // 輪郭エッジフラグ(0-1)
        // 値が0の場合有効
        dst_vertex.edge_flag = src_vertex.edge_flag == 0 ? 1.0f : 0.0f;
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
        .ByteWidth = UINT(sizeof(uint16_t) * indices->size),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_INDEX_BUFFER,
    };
    const D3D11_SUBRESOURCE_DATA init_data = {
        .pSysMem = indices->indices.data()
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

bool PMDModel::make_material_buffer(ID3D11Device* const device) {
    for(const auto& material : this->model_loader->get_materials()->materials) {
        const auto opt_material_data = PMDMaterialData::make(
            material,
            device,
            this->path
        );
        if(!opt_material_data.has_value()) {
            return false;
        }

        this->materials.emplace_back(opt_material_data.value());
    }

    for(const std::string& file_name : this->model_loader->get_toon_textures()->file_names) {
        const auto& file_path = this->path.parent_path() / file_name;
        Texture texture = Texture{
            static_cast<uint32_t>(TextureName::Toon),
            static_cast<uint32_t>(SamplerStateName::Toon)
        };

        if(!texture.load_texure(device, file_path)) {
            Logger::error(
                Logger::make_message(
                    u8"トゥーンテクスチャを読み込めませんでした\n",
                    u8"ファイル名: ",
                    file_path.u8string()
                )
            );
        }

        this->toon_textures.emplace_back(texture);
    }

    return true;
}

bool PMDModel::make_blend_state(ID3D11Device* const device) {
    constexpr D3D11_BLEND_DESC desc{
        .RenderTarget = {
            {
                .BlendEnable = TRUE,
                .SrcBlend = D3D11_BLEND_SRC_ALPHA,
                .DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
                .BlendOp = D3D11_BLEND_OP_ADD,
                .SrcBlendAlpha = D3D11_BLEND_ONE,
                .DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA,
                .BlendOpAlpha = D3D11_BLEND_OP_ADD,
                .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
            }
    }
    };

    const HRESULT hr = device->CreateBlendState(
        &desc,
        this->blend_state.GetAddressOf()
    );
    if(FAILED(hr)) {
        return false;
    }

    return true;
}

bool PMDModel::make_rigid_body(void) {
    if(!this->physics->is_initialized()) {
        return false;
    }

    const auto world = this->physics->get_mmd_world();
    for(const auto& rigid_body : this->model_loader->get_rigid_bodies()->rigid_bodies) {
        const auto node = this->bone_manager->get_bone_node(
            rigid_body.relate_bone_index
        );

        if(!world->add_rigid_body(
            rigid_body,
            node
        )) {
            return false;
        }
    }

    return true;
}

bool PMDModel::make_joint(void) {
    if(!this->physics->is_initialized()) {
        return false;
    }

    const auto world = this->physics->get_mmd_world();
    for(const auto& joint : this->model_loader->get_physics_joints()->physics_joints) {
        if(!world->add_joint(joint)) {
            return false;
        }
    }

    return true;
}