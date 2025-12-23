#include "ConstantBuffers.h"

ConstantBuffers::ConstantBuffers() {
	this->camera = Camera{};
	this->material = Material{};
}

void ConstantBuffers::update_camera(ID3D11DeviceContext* const context) {
	context->UpdateSubresource(this->camera_buffer.Get(), 0, nullptr, &this->camera, 0, 0);
}

void ConstantBuffers::make_camera(ConstantBuffers* const buffer, ID3D11Device* const device, const UINT width, const UINT height) {
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Camera);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	const HRESULT hr = device->CreateBuffer(&bd, nullptr, buffer->camera_buffer.GetAddressOf());
	if FAILED(hr) {
		throw;
	}

	buffer->camera.world = DirectX::XMMatrixIdentity();

	const float eye_position = 10.0f;
	const DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, eye_position + 2.f, 20.0f, 1.0f);
	const DirectX::XMVECTOR target = DirectX::XMVectorSet(0.0f, eye_position, 0.0f, 1.0f);
	const DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	buffer->camera.view = DirectX::XMMatrixLookAtLH(eye, target, up);

	// ‰“‹ßŠ´
	buffer->camera.projection = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(60.0f),
		float(width) / float(height),
		0.1f,
		1000.0f
	);

	// DirectX‚È‚Ì‚Å—ñ—Dæ
	buffer->camera.world = DirectX::XMMatrixTranspose(buffer->camera.world);
	buffer->camera.view = DirectX::XMMatrixTranspose(buffer->camera.view);
	buffer->camera.projection = DirectX::XMMatrixTranspose(buffer->camera.projection);
}

ConstantBuffers ConstantBuffers::make_buffer(ID3D11Device* const device, const UINT width, const UINT height) {
	ConstantBuffers buffer{};

	ConstantBuffers::make_camera(&buffer, device, width, height);

	return buffer;
}

void ConstantBuffers::update_buffers(ID3D11DeviceContext* const context) {
	this->update_camera(context);
}

void ConstantBuffers::render_set_buffers(ID3D11DeviceContext* const context) const {
	context->VSSetConstantBuffers(0, 1, this->camera_buffer.GetAddressOf());
}