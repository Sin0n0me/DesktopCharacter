#pragma once
#include "../../../object/IObjectRenderer.h"
#include "../bone/Bone.h"
#include "PMDFileStruct.h"
#include <memory>
#include <string>
#include <vector>
#include <wrl/client.h>
#include "IBoneAccessor.h"

struct ID3D11Buffer;
class PMDModelLoader;

class PMDBoneManager : public IObjectRenderer, public IBoneAccessor {
private:
	std::shared_ptr<Bones> bones; // çXêVóp
	std::shared_ptr<PMDBoneMap> bone_map;
	std::shared_ptr<IKSolver> ik_soulver;
	std::unordered_map<std::string, BoneIndex> bone_name_map;
	Microsoft::WRL::ComPtr<ID3D11Buffer> bone_buffer;

	bool make_constant_buffer(ID3D11Device* const device);

public:

	explicit PMDBoneManager(
		const std::vector<PMDBone>& bones,
		const std::vector<PMDIK>& iks
	) noexcept;

	bool init(ID3D11Device* const device) override;
	void update(ID3D11DeviceContext* const context) override;
	void render(ID3D11DeviceContext* const context, const ShaderBindingSlots* slots) const override;

	std::shared_ptr<Bones> get_mutable_bones(void) const override;
	std::shared_ptr<const IKSolver> get_ik_soulver(void) const override;
	BoneIndex get_bone_index(const std::string& name) const override;
	const PMDBoneData& get_bone(const BoneIndex& index) const override;
	const std::unordered_map<std::string, BoneIndex>& get_bone_name_map(void) const override;
};
