#include "../../constant_buffer/Bones.h"
#include "../../constant_buffer/ConstantBufferNames.h"
#include "../../shader/BindingSlotKind.h"
#include "../../shader/ShaderBindingSlots.h"
#include "../../shader/ShaderType.h"
#include "../ik/IKSolver.h"
#include "PMDBoneManager.h"
#include "PMDModelLoader.h"
#include <d3d11.h>
#include <ranges>

PMDBoneManager::PMDBoneManager(
    const std::vector<PMDBone>& bones,
    const std::vector<PMDIK>& iks
) noexcept {
    this->bone_map = std::make_shared<PMDBoneMap>();
    this->bones = std::make_unique<Bones>();

    const auto bone_count = bones.size();
    for(int index = 0; index < bone_count; ++index) {
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

        this->bone_name_map.emplace(bone.name, index);
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

    this->ik_soulver = std::make_shared<IKSolver>(
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

    D3D11_BUFFER_DESC desc{};
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = sizeof(Bones);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA init_data{};
    init_data.pSysMem = this->bones.get();
    init_data.SysMemPitch = 0;
    init_data.SysMemSlicePitch = sizeof(Bones);

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

    // TODO: 更新された部分だけのコピー
    memcpy(mapped.pData, this->bones.get(), sizeof(Bones));
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

std::shared_ptr<Bones> PMDBoneManager::get_mutable_bones(void) const {
    return this->bones;
}

std::shared_ptr<const IKSolver> PMDBoneManager::get_ik_soulver(void) const {
    return this->ik_soulver;
}

BoneIndex PMDBoneManager::get_bone_index(const std::string& name) const {
    return this->bone_name_map.at(name);
}

const PMDBoneData& PMDBoneManager::get_bone(const BoneIndex& index) const {
    return this->bone_map->at(index);
}

const std::unordered_map<std::string, BoneIndex>& PMDBoneManager::get_bone_name_map(void) const {
    return this->bone_name_map;
}