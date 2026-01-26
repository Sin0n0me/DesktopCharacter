#pragma once
#include "../../../../object/IObjectRenderer.h"
#include "../bone/Bone.h"
#include "../PMDFileStruct.h"
#include "BoneNode.h"
#include "IBoneAccessor.h"
#include <memory>
#include <string>
#include <vector>
#include <wrl/client.h>

struct ID3D11Buffer;
class PMDModelLoader;

class PMDBoneManager : public IObjectRenderer, public IBoneAccessor {
private:
    struct BoneNameProfile {
        std::string name;
        BoneIndex index;
    };

private:
    std::shared_ptr<Bones> bones; // 更新用
    std::shared_ptr<std::vector<BoneNode>> bone_matricies;// 更新用
    std::shared_ptr<PMDBoneMap> bone_map;
    std::shared_ptr<IKSolver> ik_soulver;
    std::unordered_map<uint32_t, BoneNameProfile> bone_name_map;
    Microsoft::WRL::ComPtr<ID3D11Buffer> bone_buffer;

    bool make_constant_buffer(ID3D11Device* const device);

public:

    explicit PMDBoneManager(
        const std::shared_ptr<const PMDBones>& bones,
        const std::shared_ptr<const PMDIKs>& iks
    ) noexcept;

    bool init(ID3D11Device* const device) override;
    void render_update(ID3D11DeviceContext* const context) override;
    void render(ID3D11DeviceContext* const context, const ShaderBindingSlots* slots) const override;

    std::shared_ptr<Bones> get_mutable_bones(void) const override;
    std::shared_ptr<std::vector<BoneNode>> get_mutable_bone_nodes(void) const override;
    std::shared_ptr<const IKSolver> get_ik_soulver(void) const override;
    std::optional<BoneIndex> get_bone_index(const std::string& name) const override;
    const PMDBoneData& get_bone(const BoneIndex& index) const override;
};
