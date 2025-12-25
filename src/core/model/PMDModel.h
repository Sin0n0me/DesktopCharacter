#pragma once

#include <filesystem>
#include <string>
#include <DirectXMath.h>
#include <unordered_map>
#include <wrl/client.h>
#include "IModelRenderer.h"
#include "../texrure/Texture.h"
#include "pmd/PMDFileStruct.h"

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
	std::vector<PMDVertex> vertices;
	std::vector<uint16_t> indices;
	std::vector<PMDMaterial> materials;
	std::vector<Bone> bones;
	std::vector<SkinnedVertex> skinned_vertices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	std::unordered_map<std::string, MaterialData> material_map;

	PMDModel(void) = delete;
	PMDModel(const std::filesystem::path& path) : model_file_path(path) {}

	void make_skinned_vertices();

public:
	const std::filesystem::path model_file_path;

	static std::optional<PMDModel> load_pmd(const std::filesystem::path& path);

	void make_buffers(ID3D11Device* const device);

	void update(ID3D11DeviceContext* const context) override;

	void render(ID3D11DeviceContext* const context) const override;
};
