#include "Wall.h"
#include <DirectXMath.h>

constexpr float WALL_SIZE = 50.0f;

struct ShadowReceiverVertex {
	DirectX::XMFLOAT3 position;
};

void Wall::create_shadow_receiver_mesh(ID3D11Device* const device) {
	Wall wall{};

	constexpr ShadowReceiverVertex vertices[4] = {
		{{-WALL_SIZE, -WALL_SIZE, 0.0f}},
		{{WALL_SIZE, -WALL_SIZE, 0.0f}},
		{{WALL_SIZE, WALL_SIZE, 0.0f}},
		{{-WALL_SIZE, WALL_SIZE, 0.0f}},
	};

	constexpr uint32_t indices[6] = {
		0, 1, 2,
		0, 2, 3
	};

	wall.out_index_count = sizeof(indices) / sizeof(uint32_t);

	{
		D3D11_BUFFER_DESC vb_desc{};
		vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
		vb_desc.ByteWidth = sizeof(vertices);
		vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vb_data{};
		vb_data.pSysMem = vertices;

		device->CreateBuffer(
			&vb_desc,
			&vb_data,
			wall.vertex_buffer.GetAddressOf()
		);
	}

	{
		D3D11_BUFFER_DESC ib_desc{};
		ib_desc.Usage = D3D11_USAGE_IMMUTABLE;
		ib_desc.ByteWidth = sizeof(indices);
		ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA ib_data{};
		ib_data.pSysMem = indices;

		device->CreateBuffer(
			&ib_desc,
			&ib_data,
			wall.index_buffer.GetAddressOf()
		);
	}
}

void Wall::update(ID3D11DeviceContext* const context) {
}

void Wall::render(ID3D11DeviceContext* const context) const {
}