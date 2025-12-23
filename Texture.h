#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <filesystem>

class Texture {
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resouce_view;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

	bool make_texture(ID3D11Device* const device, const UINT width, const  UINT height, const D3D11_SUBRESOURCE_DATA& data);

	bool make_sampler(ID3D11Device* const device);

public:

	void load_texure(ID3D11Device* const device, const std::filesystem::path path);

	void render_set_resource(ID3D11DeviceContext* const context) const;
};
