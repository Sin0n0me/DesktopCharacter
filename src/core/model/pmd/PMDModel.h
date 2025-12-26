#pragma once

#include <filesystem>
#include <string>
#include <DirectXMath.h>
#include <unordered_map>
#include <wrl/client.h>
#include "../../texrure/Texture.h"
#include "../IModelRenderer.h"
#include "PMDFileStruct.h"
#include "../../collider/OBB.h"

struct Bone {
	std::string name;
	int parent;
	DirectX::XMFLOAT3 position;
	DirectX::XMMATRIX local;
	DirectX::XMMATRIX global;
	DirectX::XMMATRIX inverse_bind;
};

struct SkinnedVertex {
	float position[3];
	float normal[3];
	float uv[2];
	uint16_t bone[2];
	float weight[2];
};

struct ID3D11Device;
struct ID3D11Buffer;

class PMDModel : public IModelRenderer {
private:
	struct MaterialData {
		Texture texture;
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	};

private:
	std::vector<uint16_t> indices;
	std::vector<PMDMaterial> materials;
	std::vector<Bone> bones;
	std::vector<SkinnedVertex> vertices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	std::unordered_map<std::string, MaterialData> material_map;
	std::unordered_map<int, OBB> obb_map;

private:
	bool load_pmd(const std::filesystem::path& path);
	bool make_skinned_vertices();
	bool make_buffers(ID3D11Device* const device);
	bool compute_obb();
	OBB make_obb(const std::vector<DirectX::XMFLOAT3>& positions, const DirectX::XMVECTOR& mean, const DirectX::XMVECTOR& axis0);

public:
	const std::filesystem::path model_file_path;

public:
	PMDModel(const std::filesystem::path& path) : model_file_path(path) {}

	bool init(void) override;
	bool load_model(const std::filesystem::path& path, ID3D11Device* const device) override;
	void unload_model(void) override;
	bool is_loaded_model(void) override;
	void update(ID3D11DeviceContext* const context) override;
	void render(ID3D11DeviceContext* const context) const override;
};
