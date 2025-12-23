#include "PMDModel.h"
#include <d3d11.h>
#include <fstream>
#include <optional>
#include <vector>
#include "Material.h"

#pragma pack(push, 1)
struct PMDHeader {
	char magic[3];      // "Pmd"
	float version;      // 1.0
	char modelName[20];
	char comment[256];
};
#pragma pack(pop)

struct Vertex {
	float pos[3];
	float normal[3];
	float uv[2];
};

enum PMDBoneType {
	Rotate = 0,
	RotateMove = 1,
	IK = 2,
	IKChild = 3,
	RotateChild = 4,
	IKTarget = 6,
};

// 先頭から順に
// ヘッダー(284Byte)
// 頂点数(4Byte)
// 頂点データ(40Byte * 頂点数)
// インデックス数(4Byte)
// インデックスデータ(2Byte * インデックス数)
// マテリアル数(4Byte)
// マテリアルデータ(70Byte)
// ボーン数(2Byte)
// ボーンデータ
// IK数
// IKデータ
std::optional<PMDModel> PMDModel::load_pmd(const std::filesystem::path& path) {
	// 念のためファイルが存在するかチェック
	if(!std::filesystem::exists(path)) {
		return std::optional<PMDModel>();
	}

	std::ifstream file(path, std::ios::binary | std::ios::in);

	if(!file.is_open()) {
		return std::optional<PMDModel>();
	}

	// ヘッダ読み込み
	PMDHeader header{};
	file.read(reinterpret_cast<char*>(&header), sizeof(header));
	if(std::strncmp(header.magic, "Pmd", 3) != 0) {
		return std::optional<PMDModel>();
	}

	// モデルデータの読み込み開始
	// TODO: reinterpret_cast使ったりとあんまり安全ではないので変更したい
	PMDModel model(path.string());

	// vertices
	{
		uint32_t vertex_count{};
		file.read(reinterpret_cast<char*>(&vertex_count), sizeof(vertex_count));

		model.vertices.resize(vertex_count);
		file.read(
			reinterpret_cast<char*>(model.vertices.data()),
			sizeof(PMDVertex) * vertex_count
		);

		// 座標系が違うのでDirectXの左手系に変更
		for(auto& vertex : model.vertices) {
			vertex.pos[2] *= -1.0f;
			vertex.normal[2] *= -1.0f;
		}

		model.skinned_vertices.resize(model.vertices.size());
		for(size_t i = 0; i < model.vertices.size(); ++i) {
			auto& src = model.vertices[i];
			auto& dest = model.skinned_vertices[i];

			memcpy(dest.position, src.pos, sizeof(float) * 3);
			memcpy(dest.normal, src.normal, sizeof(float) * 3);
			memcpy(dest.uv, src.uv, sizeof(float) * 2);

			dest.bone[0] = src.bone_index[0];
			dest.bone[1] = src.bone_index[1];

			dest.weight[0] = src.bone_weight / 100.0f;
			dest.weight[1] = 1.0f - dest.weight[0];
		}
	}

	// indices
	{
		uint32_t index_count{};
		file.read(reinterpret_cast<char*>(&index_count), sizeof(index_count));

		model.indices.resize(index_count);
		file.read(
			reinterpret_cast<char*>(model.indices.data()),
			sizeof(uint16_t) * index_count
		);

		// 座標系が違うのでDirectXの左手系に変更
		for(size_t i = 0; i < model.indices.size(); i += 3) {
			std::swap(model.indices[i + 1], model.indices[i + 2]);
		}
	}

	// materials
	{
		uint32_t material_count{};
		file.read(reinterpret_cast<char*>(&material_count), 4);

		model.materials.resize(material_count);
		file.read(
			reinterpret_cast<char*>(model.materials.data()),
			sizeof(PMDMaterial) * material_count
		);
	}

	// bone
	{
		uint16_t bone_count{};
		file.read(reinterpret_cast<char*>(&bone_count), sizeof(bone_count));

		std::vector<PMDBone> raw(bone_count);
		file.read(
			reinterpret_cast<char*>(raw.data()),
			sizeof(PMDBone) * bone_count
		);

		model.bones.resize(bone_count);

		for(int i = 0; i < bone_count; ++i) {
			model.bones[i].name = raw[i].name;
			model.bones[i].parent = (raw[i].parent_index == 0xFFFF) ? -1 : raw[i].parent_index;
			model.bones[i].position = {
				raw[i].position[0],
				raw[i].position[1],
				raw[i].position[2]
			};

			// ローカルからグローバルへの変換
			DirectX::XMMATRIX local = DirectX::XMMatrixTranslation(
				model.bones[i].position.x,
				model.bones[i].position.y,
				model.bones[i].position.z
			);
			if(model.bones[i].parent >= 0) {
				model.bones[i].global = local * model.bones[model.bones[i].parent].global;
			} else {
				model.bones[i].global = local;
			}
		}

		for(auto& bone : model.bones) {
			bone.inverse_bind = DirectX::XMMatrixInverse(nullptr, bone.global);
		}
	}

	// IK
	{
		uint16_t ik_count{};
		file.read((char*)&ik_count, sizeof(ik_count));

		std::vector<PMDIK> iks(ik_count);
		for(int i = 0; i < ik_count; i++) {
			PMDIK raw;
			// 可変部分であるchainの部分だけは後で読み込むように
			// 先頭11Byteだけ読み込み
			file.read((char*)&raw, sizeof(PMDIK) - sizeof(PMDIK::chain));
			iks[i].ik_bone = raw.ik_bone;
			iks[i].target_bone = raw.target_bone;
			iks[i].iterations = raw.iterations;
			iks[i].limit = raw.limit;

			// 可変部分
			iks[i].chain.resize(raw.chain_length);
			file.read(
				(char*)iks[i].chain.data(),
				sizeof(uint16_t) * raw.chain_length
			);
		}
	}

	return std::optional<PMDModel>(model);
}

void PMDModel::make_buffers(ID3D11Device* const device) {
	std::vector<Vertex> vertices(this->vertices.size());

	for(size_t i = 0; i < this->vertices.size(); ++i) {
		memcpy(vertices[i].pos, this->vertices[i].pos, sizeof(float) * 3);
		memcpy(vertices[i].normal, this->vertices[i].normal, sizeof(float) * 3);
		memcpy(vertices[i].uv, this->vertices[i].uv, sizeof(float) * 2);
	}

	// 頂点バッファ作成
	{
		D3D11_BUFFER_DESC vbDesc = {};
		vbDesc.Usage = D3D11_USAGE_DEFAULT;
		vbDesc.ByteWidth = UINT(sizeof(Vertex) * vertices.size());
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vbData = {};
		vbData.pSysMem = vertices.data();

		const HRESULT hr = device->CreateBuffer(&vbDesc, &vbData, this->vertex_buffer.GetAddressOf());
		if FAILED(hr) {
			throw;
		}
	}

	// インデックスバッファ作成
	{
		D3D11_BUFFER_DESC ibDesc = {};
		ibDesc.Usage = D3D11_USAGE_DEFAULT;
		ibDesc.ByteWidth = UINT(sizeof(uint16_t) * this->indices.size());
		ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA ibData = {};
		ibData.pSysMem = this->indices.data();

		const HRESULT hr = device->CreateBuffer(&ibDesc, &ibData, this->index_buffer.GetAddressOf());
		if FAILED(hr) {
			throw;
		}
	}

	// テクスチャ生成
	// 定数バッファ作成
	for(const auto& material : this->materials) {
		MaterialData material_data{};

		{
			// スフィアがついている場合があるので分離
			std::string	texture_path = material.texture_file;
			const auto pos = texture_path.find('*');
			if(pos != std::string::npos) {
				texture_path = texture_path.substr(0, pos);
			}

			material_data.texture.load_texure(
				device,
				this->model_file_path.parent_path().append(texture_path)
			);
		}

		{
			D3D11_BUFFER_DESC bd{};
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(Material);
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

			Material mat{};
			memcpy(mat.diffuse, material.diffuse, sizeof(float) * 4);
			memcpy(mat.ambient, material.ambient, sizeof(float) * 3);

			D3D11_SUBRESOURCE_DATA init_data{};
			init_data.pSysMem = &mat;
			init_data.SysMemPitch = 0;
			init_data.SysMemSlicePitch = sizeof(Material);

			const HRESULT hr = device->CreateBuffer(
				&bd,
				&init_data,
				material_data.buffer.GetAddressOf()
			);
			if FAILED(hr) {
				throw;
			}
		}

		this->material_map.emplace(material.texture_file, material_data);
	}
}

void PMDModel::render(ID3D11DeviceContext* const context) const {
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;

	context->IASetVertexBuffers(0, 1, this->vertex_buffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(this->index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	uint32_t index_offset = 0;
	for(const auto& material : this->materials) {
		const auto& mat = this->material_map.at(material.texture_file);
		mat.texture.render_set_resource(context);
		context->VSSetConstantBuffers(1, 1, mat.buffer.GetAddressOf());

		context->DrawIndexed(material.index_count, index_offset, 0);
		index_offset += material.index_count;
	}
}

void PMDModel::update(ID3D11DeviceContext* const context) {
}