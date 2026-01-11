#include "../../../utility/Algorithm.h"
#include "../../../utility/BinaryReader.h"
#include "../../constant_buffer/ConstantBufferNames.h"
#include "../../constant_buffer/Material.h"
#include "../../motion/vmd/VMDMotion.h"
#include "../../shader/Shader.h"
#include "../../shader/ShaderBindingSlots.h"
#include "PMDModel.h"
#include "PMDModelLoader.h"
#include <d3d11.h>

constexpr float WEIGHT_THRESHOLD = 0.3f;

struct Vertex {
	float position[3];
	float normal[3];
	float uv[2];
	uint16_t bone_index[2];	// ボーン番号
	float bone_weight[2]; // 0-1
};

PMDModel::PMDModel(const std::filesystem::path& path) : Model(path) {
	this->model_loader = std::make_shared<PMDModelLoader>(path);
}

Model::Model(const std::filesystem::path& path) : path(path) {}

void PMDModel::on_clicked(const short obb_index) {
}

bool PMDModel::init(ID3D11Device* const device) {
	if(this->is_loaded_model()) {
		return true;
	}

	// pmdの読み込み
	if(!this->model_loader->load_pmd()) {
		return false;
	}

	// バッファ作成
	if(!this->make_buffers(device)) {
		return false;
	}

	// ボーン名の解決
	this->bone_manager = std::make_shared<PMDBoneManager>(
		this->model_loader->get_bones(),
		this->model_loader->get_iks()
	);
	if(!this->bone_manager->init(device)) {
		return false;
	}

	//
	this->motion_manager = std::make_shared<VMDMotionManager>(this->bone_manager);
	if(!this->motion_manager->init()) {
		return false;
	}

	return true;
}

void PMDModel::update(ID3D11DeviceContext* const context) {
	this->bone_manager->update(context);
}

void PMDModel::render(
	ID3D11DeviceContext* const context,
	const ShaderBindingSlots* slots
) const {
	this->bone_manager->render(context, slots);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		this->vertex_buffer.GetAddressOf(),
		&stride,
		&offset
	);
	context->IASetIndexBuffer(
		this->index_buffer.Get(),
		DXGI_FORMAT_R16_UINT,
		0
	);

	uint32_t index_offset = 0;
	for(const auto& material : this->materials) {
		material.texture.render_set_resource(context, slots);

		if(material.sphere_mode != SphereMode::None) {
			material.sphere.render_set_resource(context, slots);
		}

		// シャドウマップなどで使用しない場合もある
		const BindingSlotKey key = BindingSlotKey{
			static_cast<uint32_t>(ConstantBufferName::Material),
			ShaderType::Pixel,
			BindingSlotKind::ConstantBuffer,
		};
		if(slots->contains(key)) {
			context->PSSetConstantBuffers(
				slots->get(key),
				1,
				material.buffer.GetAddressOf()
			);
		}

		context->DrawIndexed(material.index_count, index_offset, 0);
		index_offset += material.index_count;
	}
}

void PMDModel::unload_model() {
}

bool PMDModel::is_loaded_model() {
	return bool(this->bone_manager) && bool(this->motion_manager);
}

// 主成分分析による最小体積のOBBを作成
void PMDModel::compute_obb(std::unordered_map<short, OBB>& obb_map) {
	// ボーンが影響する頂点のみを収集
	std::unordered_map<short, std::vector<DirectX::XMFLOAT3>> points_map;
	for(const auto& vertex : this->model_loader->get_vertices()) {
		if(vertex.bone_weight > WEIGHT_THRESHOLD) {
			points_map[vertex.bone_index[0]].emplace_back(vertex.position);
		}
	}

	// ボーンのローカル空間へ変換と共分散行列の作成
	for(auto& [index, positions] : points_map) {
		obb_map.emplace(index, OBB::make_by_covariance_matrix(positions));
	}
}

void PMDModel::update_obb(std::unordered_map<short, OBB>& obb_map) {
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

	return true;
}

bool PMDModel::make_vertex_buffer(ID3D11Device* const device) {
	const auto& loaded_vertices = this->model_loader->get_vertices();
	const auto size = loaded_vertices.size();
	std::vector<Vertex> vertices(size);
	for(size_t i = 0; i < size; ++i) {
		memcpy(
			vertices[i].position,
			loaded_vertices[i].position,
			sizeof(decltype(vertices[i].position))
		);
		memcpy(
			vertices[i].normal,
			loaded_vertices[i].normal,
			sizeof(decltype(vertices[i].normal))
		);
		memcpy(
			vertices[i].uv,
			loaded_vertices[i].uv,
			sizeof(decltype(vertices[i].uv))
		);
		memcpy(
			vertices[i].bone_index,
			loaded_vertices[i].bone_index,
			sizeof(decltype(vertices[i].bone_index))
		);

		// 正規化
		const float weight = static_cast<float>(loaded_vertices[i].bone_weight) / 100.0f;
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
	const auto& indices = this->model_loader->get_indices();
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = UINT(sizeof(uint16_t) * indices.size());
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = indices.data();

	const HRESULT hr = device->CreateBuffer(
		&desc,
		&init_data,
		this->index_buffer.GetAddressOf()
	);
	if FAILED(hr) {
		return false;
	}

	return true;
}

bool PMDModel::make_constant_buffer(ID3D11Device* const device) {
	for(const auto& material : this->model_loader->get_materials()) {
		PMDMaterialData material_data{};

		// index
		material_data.index_count = material.index_count;

		// スフィアがついている場合があるので分離
		std::string	texture_path = material.texture_file;
		const auto pos = texture_path.find('*');
		if(pos == std::string::npos) {
			material_data.sphere_mode = SphereMode::None;
		} else {
			std::string sphere = texture_path.substr(pos + 1);
			texture_path = texture_path.substr(0, pos);

			if(sphere.ends_with(".sph")) {
				material_data.sphere_mode = SphereMode::Multiply;
			} else if(sphere.ends_with(".spa")) {
				material_data.sphere_mode = SphereMode::Add;
			} else {
				return false;
			}

			if(!material_data.sphere.load_texure(
				device,
				this->path.parent_path().append(sphere)
			)) {
				return false;
			}
		}

		if(!material_data.texture.load_texure(
			device,
			this->path.parent_path().append(texture_path)
		)) {
			return false;
		}

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

		this->materials.emplace_back(material_data);
	}

	return true;
}

void PMDModel::update_motion(const int64_t delta_time) {
	this->motion_manager->update_motion(delta_time);
}