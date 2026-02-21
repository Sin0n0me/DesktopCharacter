#include "../../../constant_buffer/Bones.h"
#include "../../../constant_buffer/ConstantBufferNames.h"
#include "../../../shader/BindingSlotKind.h"
#include "../../../shader/ShaderBindingSlots.h"
#include "../../../shader/ShaderType.h"
#include "../PMDModelLoader.h"
#include "BoneNode.h"
#include "PMDBoneManager.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <map>

PMDBoneManager::PMDBoneManager(
    const std::shared_ptr<const PMDBones>& bones
) noexcept :
    bones(new Bones()),
    bone_nodes() {
    const auto bone_count = bones->size;
    std::map<BoneIndex, BindBone> bind_bone_map;
    for(uint16_t index = 0; index < bone_count; ++index) {
        const auto& bone = bones->bones.at(index);
        auto bind_bone = BindBone{};

        bind_bone.position = {
            bone.position[0],
            bone.position[1],
            bone.position[2]
        };

        const auto parent_index = bone.parent_index;
        if(parent_index == 0xFFFF) {
            bind_bone.local = MMDMatrix::make_translation_from_vector(
                bind_bone.position
            );
        } else {
            const auto& parent = bind_bone_map.at(parent_index);
            bind_bone.local = MMDMatrix::make_translation_from_vector(
                DirectX::XMVectorSubtract(
                    bind_bone.position,
                    parent.position
                )
            );
        }

        bind_bone_map.emplace(
            index,
            bind_bone
        );

        this->bone_name_map.emplace(
            hash_u32(bone.name),
            BoneNameProfile{
                .name = bone.name,
                .index = index,
            }
            );
    }

    this->bone_nodes.resize(bind_bone_map.size());
    for(auto& [index, bind_bone] : bind_bone_map) {
        const auto& bone = bones->bones.at(index);
        const auto parent_index = bone.parent_index;
        if(parent_index == 0xFFFF) {
            bind_bone.global = bind_bone.local;
        } else {
            const auto& parent = bind_bone_map.at(parent_index);
            bind_bone.global = bind_bone.local * parent.global;
        }

        // 逆変換
        bind_bone.global_inverse = bind_bone.global.inverse();
        this->bones->bone_matrices[index] = bind_bone.global_inverse * bind_bone.global;

        // ボーンノードの作成
        if(parent_index == 0xFFFF) {
            auto node = BoneNode::make(bind_bone);
            this->bone_nodes[index] = node;
        } else {
            const auto& parent_node = this->bone_nodes.at(parent_index);
            auto node = BoneNode::make(bind_bone, parent_node);

            this->bone_nodes[index] = node;
        }

        const auto& node = this->bone_nodes.at(index);
        node->update_local();
    }

    for(const auto& node : this->get_root_bones()) {
        node->update_global();
    }
}

bool PMDBoneManager::make_constant_buffer(ID3D11Device* const device) {
    constexpr D3D11_BUFFER_DESC desc{
        .ByteWidth = sizeof(Bones),
        .Usage = D3D11_USAGE_DYNAMIC,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    };
    const D3D11_SUBRESOURCE_DATA init_data{
        .pSysMem = this->bones.get(),
        .SysMemPitch = 0,
        .SysMemSlicePitch = sizeof(Bones),
    };

    const HRESULT hr = device->CreateBuffer(
        &desc,
        &init_data,
        this->bone_buffer.GetAddressOf()
    );
    if FAILED(hr) {
        return false;
    }

    return true;
}

bool PMDBoneManager::init(ID3D11Device* const device) {
    if(!this->make_constant_buffer(device)) {
        return false;
    }

    return true;
}

void PMDBoneManager::render_update(ID3D11DeviceContext* const context) {
    const auto size = this->bone_nodes.size();
    for(size_t i = 0; i < size; ++i) {
        this->bones->bone_matrices[i] = this->bone_nodes.at(i)->get_global();
    }

    D3D11_MAPPED_SUBRESOURCE mapped;
    context->Map(this->bone_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, this->bones.get(), sizeof(decltype(this->bones)::element_type));
    context->Unmap(this->bone_buffer.Get(), 0);
}

void PMDBoneManager::render(
    ID3D11DeviceContext* const context,
    const ShaderBindingSlots* slots
) const {
    context->VSSetConstantBuffers(
        slots->get(
            ShaderType::Vertex,
            BindingSlotKind::ConstantBuffer,
            static_cast<uint32_t>(ConstantBufferName::Bones)
        ),
        1,
        this->bone_buffer.GetAddressOf()
    );
}

const std::vector<BoneNodePtr>& PMDBoneManager::get_all_bone_nodes(void) const {
    return this->bone_nodes;
}

BoneNodePtr PMDBoneManager::get_bone_node(const BoneIndex& index) const {
    return this->bone_nodes.at(index);
}

std::optional<BoneIndex> PMDBoneManager::get_bone_index(const std::string& name) const {
    const auto& iter = this->bone_name_map.find(hash_u32(name.c_str()));
    if(iter == this->bone_name_map.end()) {
        return std::nullopt;
    }

    return iter->second.index;
}

std::vector<BoneNodePtr> PMDBoneManager::get_root_bones(void) const {
    std::vector<BoneNodePtr> vec{};

    for(const auto& node : this->bone_nodes) {
        if(!bool(node->parent.lock())) {
            vec.push_back(node);
        }
    }

    return vec;
}