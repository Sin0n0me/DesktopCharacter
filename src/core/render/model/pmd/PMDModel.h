#pragma once

#include "../../../collider/OBB.h"
#include "../../motion/MotionState.h"
#include "../../motion/vmd/VMDMotion.h"
#include "../../texrure/Texture.h"
#include "../Model.h"
#include "PMDFileStruct.h"
#include <string>
#include <unordered_map>
#include <wrl/client.h>

struct PMDMaterialData {
	Texture texture;
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};

struct ID3D11Device;
struct ID3D11Buffer;
struct Bones;
class BinaryReader;

class PMDModel : public Model {
private:
	struct TempBuffer {
		std::vector<PMDVertex> vertices;
		std::vector<uint16_t> indices;
	};

private:
	std::unique_ptr<TempBuffer> temp;
	std::vector<PMDMaterial> materials;
	std::vector<PMDIK> iks;
	std::vector<PMDMorph> morphs;
	std::shared_ptr<Bones> bones; // 更新用
	std::shared_ptr<PMDBoneMap> bone_map; // 参照用
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> bone_buffer;
	std::unordered_map<std::string, BoneIndex> bone_name_map;
	std::unordered_map<std::string, PMDMaterialData> material_map;
	std::unordered_map<MotionState, VMDMotion> motion_map;

	MotionState current_motion;

	Microsoft::WRL::ComPtr<ID3D11Buffer> debug_vertex_buffer;

public:
	PMDModel(const std::filesystem::path& path);

	void on_clicked(const short obb_index) override;

	bool init(void) override;
	bool load_model(ID3D11Device* const device) override;
	void unload_model(void) override;
	bool is_loaded_model(void) override;
	void update_render(ID3D11DeviceContext* const context) override;
	void render(ID3D11DeviceContext* const context) const override;

	void compute_obb(std::unordered_map<short, OBB>& obb_map) override;
	void update_obb(std::unordered_map<short, OBB>& obb_map) override;

	bool make_vertex_shader(ID3D11Device* const device, ID3D11VertexShader** vertex_shader, ID3DBlob** vs_blob, ID3DBlob** error_blob) override;
	bool make_pixel_shader(ID3D11Device* const device, ID3D11PixelShader** pixel_shader, ID3DBlob** ps_blob, ID3DBlob** error_blob) override;
	bool make_input_layout(ID3D11Device* const device, ID3D11InputLayout** input_layout, ID3DBlob** vs_blob) override;

	void update_motion(const int64_t delta_time) override;

private:
	// TODO: 分離
	bool load_pmd(const std::filesystem::path& path);
	bool load_pmd_header(BinaryReader& file);
	bool load_pmd_vertices(BinaryReader& file);
	bool load_pmd_indices(BinaryReader& file);
	bool load_pmd_materials(BinaryReader& file);
	bool load_pmd_bones(BinaryReader& file);
	bool load_pmd_iks(BinaryReader& file);
	bool load_pmd_morph(BinaryReader& file);

	bool make_buffers(ID3D11Device* const device);
	bool make_vertex_buffer(ID3D11Device* const device);
	bool make_index_buffer(ID3D11Device* const device);
	bool make_constant_buffer(ID3D11Device* const device);
	bool make_constant_buffer_for_textures(ID3D11Device* const device);
	bool make_constant_buffer_for_bones(ID3D11Device* const device);

	OBB make_obb(const std::vector<DirectX::XMFLOAT3>& positions, const DirectX::XMVECTOR& mean, const DirectX::XMMATRIX& eigenvector);

	bool make_motions(void);

	void clear_temp_buffers(void);
};
