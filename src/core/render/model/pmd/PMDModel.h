#pragma once

#include "../../../collider/OBB.h"
#include "../../texrure/Texture.h"
#include "../Model.h"
#include "material/PMDMaterialData.h"
#include "PMDBoneManager.h"
#include "../../motion/vmd/VMDMotionManager.h"

struct ID3D11Device;
class PMDModelLoader;

class PMDModel : public Model {
private:
	std::shared_ptr<PMDBoneManager> bone_manager;
	std::shared_ptr<PMDModelLoader> model_loader;
	std::shared_ptr<VMDMotionManager> motion_manager;
	std::vector<PMDMaterialData> materials;

public:
	explicit PMDModel(const std::filesystem::path& path);

	void on_clicked(const short obb_index) override;

	bool init(ID3D11Device* const device) override;
	void render_update(ID3D11DeviceContext* const context) override;
	void render(ID3D11DeviceContext* const context, const ShaderBindingSlots* slots) const override;
	bool is_loaded_model(void) override;
	void unload_model(void) override;

	void compute_obb(std::unordered_map<short, OBB>& obb_map) override;
	void update_obb(std::unordered_map<short, OBB>& obb_map) override;

	void update_motion(const int64_t delta_time) override;

private:

	bool load_pmd(void);
	bool make_buffers(ID3D11Device* const device);
	bool make_vertex_buffer(ID3D11Device* const device);
	bool make_index_buffer(ID3D11Device* const device);
	bool make_constant_buffer(ID3D11Device* const device);
};
