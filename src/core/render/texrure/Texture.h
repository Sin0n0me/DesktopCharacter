#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <filesystem>

class ShaderBindingSlots;

class Texture {
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resouce_view;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

	bool make_texture(
		ID3D11Device* const device,
		ID3D11Texture2D** texture,
		const UINT width,
		const UINT height,
		const D3D11_SUBRESOURCE_DATA& data
	);

	bool make_sampler(ID3D11Device* const device);

public:
	const uint32_t texture_name_hash;
	const uint32_t sampler_name_hash;

public:
	explicit Texture(
		const uint32_t texture_name_hash,
		const uint32_t sampler_name_hash
	) noexcept;

	bool load_texure(
		ID3D11Device* const device,
		const std::filesystem::path path
	);

	void render_set_resource(
		ID3D11DeviceContext* const context,
		const ShaderBindingSlots* slots
	) const;
};
