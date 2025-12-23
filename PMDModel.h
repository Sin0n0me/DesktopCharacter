#pragma once

#include <d3d11.h>
#include <filesystem>
#include <string>
#include <DirectXMath.h>
#include <unordered_map>
#include <wrl/client.h>
#include "IModelRenderer.h"
#include "Texture.h"

#pragma pack(push, 1)
struct PMDVertex {
	float pos[3];
	float normal[3];
	float uv[2];
	uint16_t bone_index[2];
	uint8_t bone_weight;   // 0-100
	uint8_t edge_flag;
};

struct PMDMaterial {
	float diffuse[4];
	float specular[3];
	float shininess;
	float ambient[3];
	uint8_t toon_index;
	uint8_t edge_flag;
	uint32_t index_count;  //
	char texture_file[20]; // "xxx.png" or "xxx.png*sph"
};

struct PMDBone {
	char name[20];          // ボーン名
	uint16_t parent_index;  // 親ボーン番号
	uint16_t tail_index;    // 表示先ボーン
	uint8_t type;           // ボーンタイプ
	uint16_t ik_parent;     // IK親
	float position[3];      // ボーン位置(モデル空間)
};

// これだけ可変要素を含む
struct PMDIK {
	uint16_t ik_bone;			 // IKボーン番号(足IK)
	uint16_t target_bone;		 // ターゲット(足首)
	uint8_t chain_length;		 // 影響ボーン数
	uint16_t iterations;		 // 反復回数
	float limit;				 // 回転制限(ラジアン)
	std::vector<uint16_t> chain; //
};

#pragma pack(pop)

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
	PMDModel(const std::filesystem::path path) : model_file_path(path) {}

	void make_skinned_vertices();

public:
	const std::filesystem::path model_file_path;

	static std::optional<PMDModel> load_pmd(const std::filesystem::path& path);

	void make_buffers(ID3D11Device* const device);

	void update(ID3D11DeviceContext* const context) override;

	void render(ID3D11DeviceContext* const context) const override;
};
