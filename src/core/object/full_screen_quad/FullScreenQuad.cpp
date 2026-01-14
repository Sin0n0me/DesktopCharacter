#include "FullScreenQuad.h"
#include <d3d11.h>

struct QuadVertex {
	float x;
	float y;
	float u;
	float v;
};

constexpr QuadVertex VERTICES[] = {
	{-1.0f, 1.0f, 0.0f, 0.0f},
	{1.0f, 1.0f, 1.0f, 0.0f},
	{-1.0f, -1.0f, 0.0f, 1.0f},
	{1.0f, -1.0f, 1.0f, 1.0f},
};

bool FullScreenQuad::init(ID3D11Device* const device) {
	{
		D3D11_BUFFER_DESC desc{};
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.ByteWidth = sizeof(VERTICES);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vb_data{};
		vb_data.pSysMem = VERTICES;

		const HRESULT hr = device->CreateBuffer(
			&desc,
			&vb_data,
			this->vertex_buffer.GetAddressOf()
		);
		if(FAILED(hr)) {
			return false;
		}
	}

	return true;
}

void FullScreenQuad::render_update(ID3D11DeviceContext* const context) {
}

void FullScreenQuad::render(
	ID3D11DeviceContext* const context,
	const ShaderBindingSlots* slots
) const {
	const UINT stride = sizeof(QuadVertex);
	const UINT offset = 0;

	context->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	);

	context->IASetVertexBuffers(
		0,
		1,
		this->vertex_buffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->Draw(4, 0);
}