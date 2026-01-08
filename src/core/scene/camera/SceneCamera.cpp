#include <Windows.h>
#include <d3d11.h>
#include "SceneCamera.h"
#include "../../../Application.h"
#include "../../render/CommonResource.h"

SceneCamera SceneCamera::make_camera(ID3D11Device* const device, const std::shared_ptr<CommonResource>& resource) {
	SceneCamera scene_camera{};

	scene_camera.camera.world = DirectX::XMMatrixIdentity();

	const float eye_position = 11.0f;
	const float distance = 25.0f;
	const DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0, eye_position + 7.5f, -distance, 1.0f);
	const DirectX::XMVECTOR target = DirectX::XMVectorSet(0.0f, eye_position, 0.0f, 1.0f);
	const DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	scene_camera.camera.view = DirectX::XMMatrixLookAtLH(eye, target, up);

	/*
	scene_camera.camera.projection = DirectX::XMMatrixOrthographicLH(
		25.0f,
		25.0f,
		0.1f,
		25.0f
	);
	*/
	// ‰“‹ßŠ´
	scene_camera.camera.projection = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(45.0f),
		float(WIDTH) / float(HEIGHT),
		0.1f,
		200.0f
	);

	// DirectX‚È‚Ì‚Å—ñ—Dæ
	scene_camera.camera.world = DirectX::XMMatrixTranspose(scene_camera.camera.world);
	scene_camera.camera.view = DirectX::XMMatrixTranspose(scene_camera.camera.view);
	scene_camera.camera.projection = DirectX::XMMatrixTranspose(scene_camera.camera.projection);

	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Camera);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA init_data{};
	init_data.pSysMem = &scene_camera.camera;
	init_data.SysMemPitch = 0;
	init_data.SysMemSlicePitch = sizeof(Camera);

	const HRESULT hr = device->CreateBuffer(
		&bd,
		&init_data,
		resource->constant_buffers[ConstantBuffer::Camera].GetAddressOf()
	);
	if FAILED(hr) {
		throw;
	}

	return scene_camera;
}