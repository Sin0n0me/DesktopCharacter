#include "../../../utility/Algorithm.h"
#include "../../../utility/BinaryReader.h"
#include "../../../utility/Utility.h"
#include "../../constant_buffer/Bones.h"
#include "../../constant_buffer/Material.h"
#include "../../motion/vmd/VMDMotion.h"
#include "PMDModel.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <optional>
#include <algorithm>
#include <ranges>

constexpr float WEIGHT_THRESHOLD = 0.3f;
constexpr wchar_t MOTION_WAVE[] = L"assets/motion/手を振る.vmd";
constexpr wchar_t VERTEX_SHADER_PATH[] = L"assets/shader/pmd_model.hlsl";
constexpr wchar_t PIXEL_SHADER_PATH[] = L"assets/shader/pmd_model.hlsl";

struct Vertex {
	float position[3];
	float normal[3];
	float uv[2];
	uint16_t bone_index[2];	// ボーン番号
	float bone_weight[2]; // 0-1
};

PMDModel::PMDModel(const std::filesystem::path& path) : Model(path) {
	this->temp = std::make_unique<TempBuffer>();
	this->bones = std::make_unique<Bones>();
	this->current_motion = MotionState::WaveHand;
	this->bone_map = std::make_shared<PMDBoneMap>();
	this->ik_soulver = std::make_shared<IKSolver>(this->bone_map);
}

void PMDModel::on_clicked(const short obb_index) {
}

bool PMDModel::init(void) {
	return true;
}

bool PMDModel::load_model(ID3D11Device* const device) {
	if(this->is_loaded_model()) {
		return true;
	}

	// pmdの読み込み
	if(!this->load_pmd(this->path)) {
		return false;
	}

	// バッファ作成
	if(!this->make_buffers(device)) {
		return false;
	}

	//
	if(!this->make_motions()) {
		return false;
	}

	// ボーン名の解決
	if(!this->motion_map.at(this->current_motion).resolve_bones(this->bone_name_map)) {
		return false;
	}

	return true;
}

void PMDModel::unload_model() {
	this->morphs.clear();
	this->materials.clear();
}

bool PMDModel::is_loaded_model() {
	return false;
}

void PMDModel::update_render(ID3D11DeviceContext* const context) {
	D3D11_MAPPED_SUBRESOURCE mapped;
	context->Map(this->bone_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	// TODO: 更新された部分だけのコピー
	memcpy(mapped.pData, this->bones.get(), sizeof(Bones));
	context->Unmap(this->bone_buffer.Get(), 0);
}

void PMDModel::render(ID3D11DeviceContext* const context) const {
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;

	context->IASetVertexBuffers(0, 1, this->vertex_buffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(this->index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->VSSetConstantBuffers(3, 1, this->bone_buffer.GetAddressOf());

	uint32_t index_offset = 0;
	for(const auto& material : this->materials) {
		const auto& mat = this->material_map.at(material.texture_file);
		mat.texture.render_set_resource(context);
		context->PSSetConstantBuffers(1, 1, mat.buffer.GetAddressOf());

		context->DrawIndexed(material.index_count, index_offset, 0);
		index_offset += material.index_count;
	}
}

// 主成分分析による最小体積のOBBを作成
void PMDModel::compute_obb(std::unordered_map<short, OBB>& obb_map) {
	// ボーンが影響する頂点のみを収集
	std::unordered_map<int, std::vector<DirectX::XMFLOAT3>>	points_map;
	for(const auto& vertex : this->temp->vertices) {
		if(vertex.bone_weight > WEIGHT_THRESHOLD) {
			points_map[vertex.bone_index[0]].emplace_back(vertex.position);
		}
	}

	// ボーンのローカル空間へ変換と共分散行列の作成
	for(auto& pair : points_map) {
		const int index = pair.first;
		auto& positions = pair.second;
		const float positions_size = static_cast<float>(positions.size());

		// 重心を求める
		DirectX::XMVECTOR mean = DirectX::XMVectorZero();
		for(const auto& position : positions) {
			mean += position;
		}
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

		// ヤコビ法による固有ベクトル取得
		DirectX::XMMATRIX eigenvectors{};
		jacobi_eigen_decomposition(matrix, eigenvectors);

		obb_map.emplace(index, this->make_obb(positions, mean, eigenvectors));
	}
}

void PMDModel::update_obb(std::unordered_map<short, OBB>& obb_map) {
}

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
	auto reader = BinaryReader::make_reader(path);
	if(!reader.has_value()) {
		return false;
	}
	auto& binary_reader = reader.value();

	// モデルデータの読み込み開始

	// ヘッダ読み込み
	if(!this->load_pmd_header(binary_reader)) {
		return false;
	}

	// vertices
	if(!this->load_pmd_vertices(binary_reader)) {
		return false;
	}

	// indices
	if(!this->load_pmd_indices(binary_reader)) {
		return false;
	}

	// materials
	if(!this->load_pmd_materials(binary_reader)) {
		return false;
	}

	// bone_matrices
	if(!this->load_pmd_bones(binary_reader)) {
		return false;
	}

	// IK
	if(!this->load_pmd_iks(binary_reader)) {
		return false;
	}

	// モーフ(表情)
	if(!this->load_pmd_morph(binary_reader)) {
		return false;
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

bool PMDModel::load_pmd_header(BinaryReader& binary_reader) {
	PMDHeader header{};
	binary_reader.read_to(&header);
	if(std::strncmp(header.magic, "Pmd", 3) != 0) {
		return false;
	}

	return true;
}

bool PMDModel::load_pmd_vertices(BinaryReader& binary_reader) {
	uint32_t vertex_count{};
	if(!binary_reader.read_to(&vertex_count)) {
		return false;
	}

	this->temp->vertices.resize(vertex_count);
	if(!binary_reader.read_to_vec(this->temp->vertices, vertex_count)) {
		return false;
	}

	// 座標系が違うのでDirectXの左手系に変更
	/*
	for(auto& vertex : this->temp->vertices) {
		vertex.position[2] *= -1.0f;
		vertex.normal[2] *= -1.0f;
	}
	*/

	return true;
}

bool PMDModel::load_pmd_indices(BinaryReader& binary_reader) {
	uint32_t index_count{};
	if(!binary_reader.read_to(&index_count)) {
		return false;
	}

	this->temp->indices.resize(index_count);
	if(!binary_reader.read_to_vec(this->temp->indices, index_count)) {
		return false;
	}

	// 座標系が違うのでDirectXの左手系に変更
	/*
	for(size_t i = 0; i < this->temp->indices.size(); i += 3) {
		std::swap(this->temp->indices[i + 1], this->temp->indices[i + 2]);
	}
	*/

	return true;
}

bool PMDModel::load_pmd_materials(BinaryReader& binary_reader) {
	uint32_t material_count{};
	if(!binary_reader.read_to(&material_count)) {
		return false;
	}

	this->materials.resize(material_count);
	if(!binary_reader.read_to_vec(this->materials, material_count)) {
		return false;
	}

	return true;
}

bool PMDModel::load_pmd_bones(BinaryReader& binary_reader) {
	uint16_t bone_count{};
	if(!binary_reader.read_to(&bone_count)) {
		return false;
	}

	std::vector<PMDBone> raw;
	if(!binary_reader.read_to_vec(raw, bone_count)) {
		return false;
	}

	for(int index = 0; index < bone_count; ++index) {
		PMDBoneData bone{};

		const auto parent_index = raw[index].parent_index;
		bone.parent = (parent_index == 0xFFFF) ? -1 : parent_index;
		bone.position = {
			raw[index].position[0],
			raw[index].position[1],
			raw[index].position[2]
		};

		if(bone.parent < 0) {
			bone.local = DirectX::XMMatrixTranslation(
				bone.position.x,
				bone.position.y,
				bone.position.z
			);
		} else {
			const auto& parent = this->bone_map->at(bone.parent);
			bone.local = DirectX::XMMatrixTranslation(
				bone.position.x - parent.position.x,
				bone.position.y - parent.position.y,
				bone.position.z - parent.position.z
			);
		}

		this->bone_name_map.emplace(raw[index].name, index);
		this->bone_map->emplace(index, bone);
	}

	// sort
	const auto& keys_view = std::views::keys(*this->bone_map);
	auto sorted_vec = std::vector<BoneIndex>(keys_view.begin(), keys_view.end());
	std::sort(sorted_vec.begin(), sorted_vec.end());

	for(auto& key : sorted_vec) {
		auto& bone = this->bone_map->at(key);
		if(bone.parent < 0) {
			bone.global = bone.local;
		} else {
			bone.global = bone.local * this->bone_map->at(bone.parent).global;
		}

		// 逆変換
		bone.inverse_bind = DirectX::XMMatrixInverse(nullptr, bone.global);
	}

	return true;
}

bool PMDModel::load_pmd_iks(BinaryReader& binary_reader) {
	uint16_t ik_count{};
	if(!binary_reader.read_to(&ik_count)) {
		return false;
	}

	std::vector<PMDIK> iks(ik_count);
	for(int i = 0; i < ik_count; i++) {
		PMDIK raw;
		// 可変部分であるchainの部分だけは後で読み込むように
		// 先頭11Byteだけ読み込み
		if(!binary_reader.read(&raw, sizeof(PMDIK) - sizeof(PMDIK::chain))) {
			return false;
		}
		auto& ik = iks[i];
		ik.ik_bone = raw.ik_bone;
		ik.target_bone = raw.target_bone;
		ik.iterations = raw.iterations;
		ik.limit = raw.limit;

		// 可変部分
		if(!binary_reader.read_to_vec(ik.chain, raw.chain_length)) {
			return false;
		}
	}

	this->ik_soulver->set_pmd_ik(std::move(iks));

	return true;
}

bool PMDModel::load_pmd_morph(BinaryReader& binary_reader) {
	uint16_t morph_count{};
	binary_reader.read_to(&morph_count);

	binary_reader.read_to_vec(this->morphs, morph_count);

	return true;
}

bool PMDModel::make_buffers(ID3D11Device* const device) {
	// 頂点バッファ作成
	if(!this->make_vertex_buffer(device)) {
		return false;
	}

	// インデックスバッファ作成
	if(!this->make_index_buffer(device)) {
		return false;
	}

	// 定数バッファ作成
	if(!this->make_constant_buffer(device)) {
		return false;
	}

	// テクスチャ生成
	if(!this->make_constant_buffer_for_textures(device)) {
		return false;
	}

	// ボーンの定数バッファ作成
	if(!this->make_constant_buffer_for_bones(device)) {
		return false;
	}

	return true;
}

bool PMDModel::make_vertex_buffer(ID3D11Device* const device) {
	std::vector<Vertex> vertices(this->temp->vertices.size());
	for(size_t i = 0; i < this->temp->vertices.size(); ++i) {
		memcpy(
			vertices[i].position,
			this->temp->vertices[i].position,
			sizeof(decltype(vertices[i].position))
		);
		memcpy(
			vertices[i].normal,
			this->temp->vertices[i].normal,
			sizeof(decltype(vertices[i].normal))
		);
		memcpy(
			vertices[i].uv,
			this->temp->vertices[i].uv,
			sizeof(decltype(vertices[i].uv))
		);
		memcpy(
			vertices[i].bone_index,
			this->temp->vertices[i].bone_index,
			sizeof(decltype(vertices[i].bone_index))
		);

		// 正規化
		const float weight = static_cast<float>(this->temp->vertices[i].bone_weight) / 100.0f;
		const float bone_weight[2] = {
			weight,
			1.0f - weight
		};
		memcpy(
			vertices[i].bone_weight,
			bone_weight,
			sizeof(decltype(vertices[i].bone_weight))
		);
	}

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = UINT(sizeof(Vertex) * vertices.size());
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = vertices.data();

	const HRESULT hr = device->CreateBuffer(&desc, &init_data, this->vertex_buffer.GetAddressOf());
	if FAILED(hr) {
		return false;
	}

	return true;
}

bool PMDModel::make_index_buffer(ID3D11Device* const device) {
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = UINT(sizeof(uint16_t) * this->temp->indices.size());
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = this->temp->indices.data();

	const HRESULT hr = device->CreateBuffer(&desc, &init_data, this->index_buffer.GetAddressOf());
	if FAILED(hr) {
		return false;
	}

	return true;
}

bool PMDModel::make_constant_buffer(ID3D11Device* const device) {
	if(!this->make_constant_buffer_for_textures(device)) {
		return false;
	}

	if(!this->make_constant_buffer_for_bones(device)) {
		return false;
	}

	return true;
}

bool PMDModel::make_constant_buffer_for_textures(ID3D11Device* const device) {
	for(const auto& material : this->materials) {
		PMDMaterialData material_data{};

		{
			// スフィアがついている場合があるので分離
			std::string	texture_path = material.texture_file;
			const auto pos = texture_path.find('*');
			if(pos != std::string::npos) {
				texture_path = texture_path.substr(0, pos);
			}

			material_data.texture.load_texure(
				device,
				this->path.parent_path().append(texture_path)
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
				return false;
			}
		}

		this->material_map.emplace(material.texture_file, material_data);
	}

	return true;
}

bool PMDModel::make_constant_buffer_for_bones(ID3D11Device* const device) {
	for(const auto& pair : *this->bone_map) {
		const auto& index = pair.first;
		const auto& bone = pair.second;

		if(index > MAX_MATRIX_SIZE - 1) {
			throw;
		}

		this->bones->bone_matrices[index] = DirectX::XMMatrixTranspose(
			bone.global * bone.inverse_bind
		);
	}

	D3D11_BUFFER_DESC desc{};
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(Bones);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA init_data{};
	init_data.pSysMem = this->bones.get();
	init_data.SysMemPitch = 0;
	init_data.SysMemSlicePitch = sizeof(Bones);

	const HRESULT hr = device->CreateBuffer(
		&desc,
		&init_data,
		this->bone_buffer.GetAddressOf()
	);
	if FAILED(hr) {
		return false;
	}

	return true;
}

bool PMDModel::make_motions(void) {
	const auto& iter = this->motion_map.emplace(
		MotionState::WaveHand,
		VMDMotion(this->bones, this->bone_map, this->ik_soulver)
	);

	if(!iter.second) {
		throw;
	}
	auto& motion = iter.first->second;

	if(!motion.init_motion()) {
		return false;
	}

	if(!motion.load_motion_file(MOTION_WAVE)) {
		return false;
	}

	return true;
}

void PMDModel::clear_temp_buffers(void) {
	this->temp->indices.clear();
	this->temp->vertices.clear();
}

bool PMDModel::make_vertex_shader(
	ID3D11Device* const device,
	ID3D11VertexShader** vertex_shader,
	ID3DBlob** vs_blob,
	ID3DBlob** error_blob
) {
	{
		const HRESULT hr = D3DCompileFromFile(
			VERTEX_SHADER_PATH,
			nullptr,
			nullptr,
			"VSMain",
			"vs_5_0",
			0, 0,
			vs_blob,
			error_blob
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	{
		const HRESULT hr = device->CreateVertexShader(
			(*vs_blob)->GetBufferPointer(),
			(*vs_blob)->GetBufferSize(),
			nullptr,
			vertex_shader
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	return true;
}

bool PMDModel::make_pixel_shader(
	ID3D11Device* const device,
	ID3D11PixelShader** pixel_shader,
	ID3DBlob** ps_blob,
	ID3DBlob** error_blob
) {
	{
		const HRESULT hr = D3DCompileFromFile(
			PIXEL_SHADER_PATH,
			nullptr,
			nullptr,
			"PSMain",
			"ps_5_0",
			0, 0,
			ps_blob,
			error_blob
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	{
		const HRESULT hr = device->CreatePixelShader(
			(*ps_blob)->GetBufferPointer(),
			(*ps_blob)->GetBufferSize(),
			nullptr,
			pixel_shader
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	return true;
}

bool PMDModel::make_input_layout(
	ID3D11Device* const device,
	ID3D11InputLayout** input_layout,
	ID3DBlob** vs_blob
) {
	const D3D11_INPUT_ELEMENT_DESC layout[] = {
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			12,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			24,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		{
			"BONEINDICES",
			0,
			DXGI_FORMAT_R16G16_UINT,
			0,
			32,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		{
			"BONEWEIGHTS",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			36,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};

	const HRESULT hr = device->CreateInputLayout(
		layout,
		_countof(layout),
		(*vs_blob)->GetBufferPointer(),
		(*vs_blob)->GetBufferSize(),
		input_layout
	);
	if(FAILED(hr)) {
		return false;
	}

	return true;
}

void PMDModel::update_motion(const int64_t delta_time) {
	this->motion_map.at(this->current_motion).update_motion(delta_time);
}

OBB PMDModel::make_obb(const std::vector<DirectX::XMFLOAT3>& positions, const DirectX::XMVECTOR& mean, const DirectX::XMMATRIX& eigen_vectors) {
	// OBBの作成
	OBB obb{};
	for(int i = 0; i < 3; ++i) {
		const DirectX::XMVECTOR axis = DirectX::XMVector3Normalize(eigen_vectors.r[i]);
		DirectX::XMStoreFloat3(&obb.axis[i], axis);
	}

	// 軸空間でAABB
	const DirectX::XMVECTOR minV = DirectX::XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, 0);
	const DirectX::XMVECTOR maxV = DirectX::XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, 0);

	constexpr int SIZE = 3;
	DirectX::XMVECTOR min_vec = DirectX::XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, 0);
	DirectX::XMVECTOR max_vec = DirectX::XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, 0);
	for(const auto& position : positions) {
		const auto& temp = (position - mean);
		const DirectX::XMVECTOR d = DirectX::XMLoadFloat3(&temp);
		const DirectX::XMVECTOR local = DirectX::XMVectorSet(
			DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, DirectX::XMLoadFloat3(&obb.axis[0]))),
			DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, DirectX::XMLoadFloat3(&obb.axis[1]))),
			DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, DirectX::XMLoadFloat3(&obb.axis[2]))),
			0.0f
		);
		min_vec = DirectX::XMVectorMin(min_vec, local);
		max_vec = DirectX::XMVectorMax(max_vec, local);
	}

	const DirectX::XMVECTOR half = sub_float(max_vec, min_vec) * 0.5f;
	const DirectX::XMVECTOR offset = add_float(max_vec, min_vec) * 0.5f;

	DirectX::XMStoreFloat3(&obb.half_extent, half);

	// 中心補正
	DirectX::XMStoreFloat3(&obb.center,
		mean +
		obb.axis[0] * offset.m128_f32[0] +
		obb.axis[1] * offset.m128_f32[1] +
		obb.axis[2] * offset.m128_f32[2]
	);

	return obb;
}