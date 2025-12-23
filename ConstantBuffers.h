#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Camera.h"
#include "Material.h"

class ConstantBuffers {
private:
	Camera camera;
	Material material;
	Microsoft::WRL::ComPtr<ID3D11Buffer> camera_buffer;

	void update_camera(ID3D11DeviceContext* const context);

	ConstantBuffers();

	static void make_camera(ConstantBuffers* const buffer, ID3D11Device* const device, const UINT width, const UINT height);

public:

	static ConstantBuffers make_buffer(ID3D11Device* const device, const UINT width, const UINT height);

	void update_buffers(ID3D11DeviceContext* const context);

	void render_set_buffers(ID3D11DeviceContext* const context) const;
};
