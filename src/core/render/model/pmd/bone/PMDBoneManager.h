#pragma once
#include "../../../../object/IObjectRenderer.h"
#include "../bone/Bone.h"
#include "../PMDFileStruct.h"
#include "IBoneAccessor.h"
#include <memory>
#include <string>
#include <vector>
#include <wrl/client.h>

struct ID3D11Buffer;
struct Bones;
class BoneNode;

class PMDBoneManager : public IObjectRenderer, public IBoneAccessor {
private:
    struct BoneNameProfile {
        std::string name;
        BoneIndex index;
    };

private:
    std::shared_ptr<Bones> bones; // 更新用
    std::vector<BoneNodePtr> bone_nodes;// 更新用
    std::unordered_map<std::string, BoneNameProfile> bone_name_map;
    Microsoft::WRL::ComPtr<ID3D11Buffer> bone_buffer;

    bool make_constant_buffer(ID3D11Device* const device);

public:

    explicit PMDBoneManager(
        const std::shared_ptr<const PMDBones>& bones
    ) noexcept;

    bool init(ID3D11Device* const device) override;
    void render_update(ID3D11DeviceContext* const context) override;
    void render(ID3D11DeviceContext* const context, const ShaderBindingSlots* slots) const override;

    const std::vector<BoneNodePtr>& get_all_bone_nodes(void) const override;
    BoneNodePtr get_bone_node(const BoneIndex& index) const override;
    std::optional<BoneIndex> get_bone_index(const std::string& name) const override;
    std::vector<BoneNodePtr> get_root_bones(void) const override;
    std::string get_bone_name(const BoneIndex& index) const override;
};
