#include "../../../../utility/Maker.h"
#include "../../../constant_buffer/Bones.h"
#include "../../../constant_buffer/ConstantBufferNames.h"
#include "../../../shader/BindingSlotKind.h"
#include "../../../shader/ShaderBindingSlots.h"
#include "../../../shader/ShaderType.h"
#include "../ik/IKSolver.h"
#include "../PMDModelLoader.h"
#include "PMDBoneManager.h"
#include <d3d11.h>
#include <ranges>

PMDBoneManager::PMDBoneManager(
    const std::vector<PMDBone>& bones,
    const std::vector<PMDIK>& iks
) noexcept {
    Maker::make_shared(this->bone_map);
    Maker::make_shared(this->bones);
    Maker::make_shared(this->bone_matricies, MAX_MATRIX_SIZE);

    for(int i = 0; i < MAX_MATRIX_SIZE; ++i) {
        this->bone_matricies->at(i) = BoneNode{
            .rotate = DirectX::XMQuaternionIdentity(),
            .local = DirectX::XMMatrixIdentity(),
            .global = DirectX::XMMatrixIdentity(),
        };
    }

    const auto bone_count = bones.size();
    for(uint16_t index = 0; index < bone_count; ++index) {
        PMDBoneData bone_data{};

        const auto& bone = bones.at(index);
        const auto parent_index = bone.parent_index;
        bone_data.parent = (parent_index == 0xFFFF) ? -1 : parent_index;
        bone_data.position = {
            bone.position[0],
            bone.position[1],
            bone.position[2]
        };

        if(bone_data.parent < 0) {
            bone_data.local = DirectX::XMMatrixTranslation(
                bone_data.position.x,
                bone_data.position.y,
                bone_data.position.z
            );
        } else {
            const auto& parent = this->bone_map->at(bone_data.parent);
            bone_data.local = DirectX::XMMatrixTranslation(
                bone_data.position.x - parent.position.x,
                bone_data.position.y - parent.position.y,
                bone_data.position.z - parent.position.z
            );
        }

        this->bone_name_map.emplace(
            hash_u32(bone.name),
            BoneNameProfile{
                .name = bone.name,
                .index = index,
            }
            );
        this->bone_map->emplace(index, bone_data);
    }

    // sort
    const auto& keys_view = std::views::keys(*this->bone_map);
    auto sorted_vec = std::vector<BoneIndex>(keys_view.begin(), keys_view.end());
    std::sort(sorted_vec.begin(), sorted_vec.end());

    for(auto& key : sorted_vec) {
        auto& bone = this->bone_map->at(key);
        if(bone.parent < 0) {
            bone.global = bone.local;
        } else {
            bone.global = bone.local * this->bone_map->at(bone.parent).global;
        }

        // 逆変換
        bone.inverse_bind = DirectX::XMMatrixInverse(nullptr, bone.global);
    }

    Maker::make_shared(
        this->ik_soulver,
        iks,
        this->bone_map
    );
}

bool PMDBoneManager::make_constant_buffer(ID3D11Device* const device) {
    for(const auto& [index, bone] : *this->bone_map) {
        if(index > MAX_MATRIX_SIZE - 1) {
            throw;
        }

        this->bones->bone_matrices[index] = DirectX::XMMatrixTranspose(
            bone.global * bone.inverse_bind
        );
    }

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

std::shared_ptr<std::vector<BoneNode>> PMDBoneManager::get_mutable_bone_nodes(void) const {
    return this->bone_matricies;
}

std::shared_ptr<Bones> PMDBoneManager::get_mutable_bones(void) const {
    return this->bones;
}

std::shared_ptr<const IKSolver> PMDBoneManager::get_ik_soulver(void) const {
    return this->ik_soulver;
}

std::optional<BoneIndex> PMDBoneManager::get_bone_index(const std::string& name) const {
    const auto& iter = this->bone_name_map.find(hash_u32(name.c_str()));
    if(iter == this->bone_name_map.end()) {
        return std::nullopt;
    }

    return iter->second.index;
}

const PMDBoneData& PMDBoneManager::get_bone(const BoneIndex& index) const {
    return this->bone_map->at(index);
}