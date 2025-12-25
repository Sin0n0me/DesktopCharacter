#pragma once
#include <wrl/client.h>

struct ID3D11ShaderResourceView;

struct CommonResource {
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadow_resouce_view;
	Microsoft::WRL::ComPtr<ID3D11Buffer> camera_constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> shadow_constant_buffer;
};
