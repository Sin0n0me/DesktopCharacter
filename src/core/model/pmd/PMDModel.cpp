#include "PMDModel.h"
#include <fstream>
#include "../../constant_buffer/Material.h"
#include "../../utility/Utility.h"

constexpr float WEIGHT_THRESHOLD = 0.3f;
constexpr int XMVECTOR_SIZE = sizeof(DirectX::XMVECTOR) / sizeof(float);

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
bool PMDModel::load_pmd(const std::filesystem::path& path) {
	// 念のためファイルが存在するかチェック
	if(!std::filesystem::exists(path)) {
		return false;
	}

	std::ifstream file(path, std::ios::binary | std::ios::in);
	if(!file.is_open()) {
		return false;
	}

	// ヘッダ読み込み
	PMDHeader header{};
	file.read(reinterpret_cast<char*>(&header), sizeof(header));
	if(std::strncmp(header.magic, "Pmd", 3) != 0) {
		return false;
	}

	// モデルデータの読み込み開始
	// TODO: reinterpret_cast使ったりとあんまり安全ではないので変更したい

	// vertices
	{
		uint32_t vertex_count{};
		file.read(reinterpret_cast<char*>(&vertex_count), sizeof(vertex_count));

		// TODO: メモリ優先か処理優先かで切り替えてもよさそう
		std::vector<PMDVertex> vertices(vertex_count);
		file.read(
			reinterpret_cast<char*>(vertices.data()),
			sizeof(PMDVertex) * vertex_count
		);

		// 座標系が違うのでDirectXの左手系に変更
		for(auto& vertex : vertices) {
			vertex.pos[2] *= -1.0f;
			vertex.normal[2] *= -1.0f;
		}

		this->vertices.resize(vertex_count);
		for(size_t i = 0; i < vertices.size(); ++i) {
			auto& src = vertices[i];
			auto& dest = this->vertices[i];

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

		this->indices.resize(index_count);
		file.read(
			reinterpret_cast<char*>(this->indices.data()),
			sizeof(uint16_t) * index_count
		);

		// 座標系が違うのでDirectXの左手系に変更
		for(size_t i = 0; i < this->indices.size(); i += 3) {
			std::swap(this->indices[i + 1], this->indices[i + 2]);
		}
	}

	// materials
	{
		uint32_t material_count{};
		file.read(reinterpret_cast<char*>(&material_count), 4);

		this->materials.resize(material_count);
		file.read(
			reinterpret_cast<char*>(this->materials.data()),
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

		this->bones.resize(bone_count);

		for(int i = 0; i < bone_count; ++i) {
			this->bones[i].name = raw[i].name;
			this->bones[i].parent = (raw[i].parent_index == 0xFFFF) ? -1 : raw[i].parent_index;
			this->bones[i].position = {
				raw[i].position[0],
				raw[i].position[1],
				raw[i].position[2]
			};

			// ローカルからグローバルへの変換
			DirectX::XMMATRIX local = DirectX::XMMatrixTranslation(
				this->bones[i].position.x,
				this->bones[i].position.y,
				this->bones[i].position.z
			);
			if(this->bones[i].parent >= 0) {
				this->bones[i].global = local * this->bones[this->bones[i].parent].global;
			} else {
				this->bones[i].global = local;
			}
		}

		// 逆変換
		for(auto& bone : this->bones) {
			bone.inverse_bind = DirectX::XMMatrixInverse(nullptr, bone.global);
		}
	}

	// 当たり判定用に別スレッドで計算を行う
	this->compute_obb();

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

	// モーフ(表情)
	{
	}

	// 英名対応
	{
	}

	// トゥーンテクスチャリスト
	{
	}

	// 物理剛体
	{
	}

	return true;
}

bool PMDModel::make_buffers(ID3D11Device* const device) {
	// 頂点バッファ作成
	{
		std::vector<Vertex> vertices(this->vertices.size());
		for(size_t i = 0; i < this->vertices.size(); ++i) {
			memcpy(vertices[i].pos, this->vertices[i].position, sizeof(float) * 3);
			memcpy(vertices[i].normal, this->vertices[i].normal, sizeof(float) * 3);
			memcpy(vertices[i].uv, this->vertices[i].uv, sizeof(float) * 2);
		}

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
		D3D11_BUFFER_DESC desc = {};
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = UINT(sizeof(uint16_t) * this->indices.size());
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA ibData = {};
		ibData.pSysMem = this->indices.data();

		const HRESULT hr = device->CreateBuffer(&desc, &ibData, this->index_buffer.GetAddressOf());
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
			D3D11_BUFFER_DESC desc{};
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = sizeof(Material);
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

			Material mat{};
			memcpy(mat.diffuse, material.diffuse, sizeof(float) * 4);
			memcpy(mat.ambient, material.ambient, sizeof(float) * 3);

			D3D11_SUBRESOURCE_DATA init_data{};
			init_data.pSysMem = &mat;
			init_data.SysMemPitch = 0;
			init_data.SysMemSlicePitch = sizeof(Material);

			const HRESULT hr = device->CreateBuffer(
				&desc,
				&init_data,
				material_data.buffer.GetAddressOf()
			);
			if FAILED(hr) {
				throw;
			}
		}

		this->material_map.emplace(material.texture_file, material_data);
	}

	return true;
}

// 主成分分析による最小体積のOBBを作成
bool PMDModel::compute_obb() {
	// ボーンが影響する頂点のみを収集
	std::unordered_map<int, std::vector<DirectX::XMFLOAT3>>	points_map;
	for(const auto& vertex : this->vertices) {
		if(vertex.weight[0] > WEIGHT_THRESHOLD) {
			points_map[vertex.bone[0]].emplace_back(vertex.position);
		}
	}

	// ボーンのローカル空間へ変換と共分散行列の作成
	for(auto& pair : points_map) {
		const int index = pair.first;
		auto& positions = pair.second;

		// 次の共分散行列作成用に先に初期化
		DirectX::XMVECTOR mean = DirectX::XMVectorZero();

		// ローカル空間への変換
		for(auto& position : positions) {
			const DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&position);
			const DirectX::XMVECTOR v2 = DirectX::XMVector3Transform(v1, this->bones[index].inverse_bind);
			DirectX::XMStoreFloat3(&position, v2);
			mean += position;
		}

		// 重心を求める
		const float positions_size = static_cast<float>(positions.size());
		mean /= positions_size;

		// 共分散行列の作成
		DirectX::XMMATRIX matrix{};
		for(const auto& position : positions) {
			const DirectX::XMFLOAT3 df = position - mean;
			matrix.r[0].m128_f32[0] += df.x * df.x;
			matrix.r[0].m128_f32[1] += df.x * df.y;
			matrix.r[0].m128_f32[2] += df.x * df.z;
			matrix.r[1].m128_f32[0] += df.y * df.x;
			matrix.r[1].m128_f32[1] += df.y * df.y;
			matrix.r[1].m128_f32[2] += df.y * df.z;
			matrix.r[2].m128_f32[0] += df.z * df.x;
			matrix.r[2].m128_f32[1] += df.z * df.y;
			matrix.r[2].m128_f32[2] += df.z * df.z;
		}
		matrix *= 1.0f / positions_size;
		matrix.r[3].m128_f32[3] = 1.0f; // 元の行列は単位行列ではないので

		// 固有ベクトル取得
		DirectX::XMVECTOR eigenvector = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		for(int i = 0; i < 16; ++i) {
			eigenvector = DirectX::XMVector3Transform(eigenvector, matrix);
			eigenvector = DirectX::XMVector3Normalize(eigenvector);
		}

		this->obb_map[index] = make_obb(positions, mean, eigenvector);
	}

	return true;
}

OBB PMDModel::make_obb(const std::vector<DirectX::XMFLOAT3>& positions, const DirectX::XMVECTOR& mean, const DirectX::XMVECTOR& axis0) {
	// OBBの作成
	const DirectX::XMVECTOR temp = fabs(DirectX::XMVectorGetX(axis0)) < 0.9f
		? DirectX::XMVectorSet(1, 0, 0, 0)
		: DirectX::XMVectorSet(0, 1, 0, 0);
	const DirectX::XMVECTOR axis1 = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(axis0, temp));
	const DirectX::XMVECTOR axis2 = DirectX::XMVector3Cross(axis0, axis1);

	float min[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
	float max[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

	for(const auto& position : positions) {
		const auto& temp = (position - mean);
		const DirectX::XMVECTOR d = DirectX::XMLoadFloat3(&temp);
		constexpr int PROJECTION_SIZE = 3;
		const float proj[PROJECTION_SIZE] =
		{
			DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, axis0)),
			DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, axis1)),
			DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, axis2))
		};

		for(int i = 0; i < PROJECTION_SIZE; ++i) {
			min[i] = min(min[i], proj[i]);
			max[i] = max(max[i], proj[i]);
		}
	}

	DirectX::XMVECTOR center{mean};
	const DirectX::XMVECTOR add_list[] = {
		axis0 * ((min[0] + max[0]) * 0.5f),
		axis1 * ((min[0] + max[0]) * 0.5f),
		axis2 * ((min[0] + max[0]) * 0.5f),
	};
	for(const auto& add : add_list) {
		center.m128_f32[0] += add.m128_f32[0];
		center.m128_f32[1] += add.m128_f32[1];
		center.m128_f32[2] += add.m128_f32[2];
		center.m128_f32[3] += add.m128_f32[3];
	}

	OBB obb{};
	DirectX::XMStoreFloat3(&obb.center, center);
	DirectX::XMStoreFloat3(&obb.axis[0], axis0);
	DirectX::XMStoreFloat3(&obb.axis[1], axis1);
	DirectX::XMStoreFloat3(&obb.axis[2], axis2);

	obb.half_extent =
	{
		(max[0] - min[0]) * 0.5f,
		(max[1] - min[1]) * 0.5f,
		(max[2] - min[2]) * 0.5f
	};

	// 安全マージン
	obb.half_extent.x *= EXTENT_MARGIN;
	obb.half_extent.y *= EXTENT_MARGIN;
	obb.half_extent.z *= EXTENT_MARGIN;

	return obb;
}

bool PMDModel::load_model(const std::filesystem::path& path, ID3D11Device* const device) {
	if(this->is_loaded_model()) {
		return true;
	}

	if(!this->load_pmd(path)) {
		return false;
	}

	if(!this->make_buffers(device)) {
		return false;
	}

	return true;
}

void PMDModel::unload_model() {
}

bool PMDModel::is_loaded_model() {
	return false;
}

bool PMDModel::init(void) {
	return false;
}

void PMDModel::update(ID3D11DeviceContext* const context) {
	for(const auto& material : this->materials) {
		const auto& material_data = this->material_map.at(material.texture_file);

		Material mat{};
		memcpy(mat.diffuse, material.diffuse, sizeof(float) * 4);
		memcpy(mat.ambient, material.ambient, sizeof(float) * 3);
		context->UpdateSubresource(
			material_data.buffer.Get(),
			0,
			nullptr,
			&mat,
			0,
			0
		);
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
		context->PSSetConstantBuffers(1, 1, mat.buffer.GetAddressOf());

		context->DrawIndexed(material.index_count, index_offset, 0);
		index_offset += material.index_count;
	}
}