#include "Light.h"
#include <d3d11.h>
#include "../../render_pass/CommonResource.h"

Light Light::make_light(ID3D11Device* const device, const std::shared_ptr<CommonResource>& common_resource) {
	Light light{};

	// ŒõŒ¹
	const DirectX::XMVECTOR light_direction = DirectX::XMVector3Normalize(
		DirectX::XMVectorSet(5.0f, -20.0f, -10.0f, 0.0f)
	);
	const DirectX::XMVECTOR light_position = DirectX::XMVectorScale(
		light_direction,
		-100.0f
	);
	const DirectX::XMMATRIX light_view = DirectX::XMMatrixLookAtLH(
		light_position,
		DirectX::XMVectorZero(),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	);
	const DirectX::XMMATRIX light_projection = DirectX::XMMatrixOrthographicLH(
		50.0f,
		50.0f,
		1.0f,
		100.0f
	);

	light.shadow.light_view_proj = DirectX::XMMatrixTranspose(light_view * light_projection);

	D3D11_BUFFER_DESC desc{};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(Shadow);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA init_data{};
	init_data.pSysMem = &light.shadow;
	init_data.SysMemPitch = 0;
	init_data.SysMemSlicePitch = sizeof(Shadow);

	const HRESULT hr = device->CreateBuffer(
		&desc,
		&init_data,
		common_resource->shadow_constant_buffer.GetAddressOf()
	);
	if FAILED(hr) {
		throw;
	}

	return light;
}