#pragma once
#include <wrl/client.h>
#include <unordered_map>

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
struct ID3D11InputLayout;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct ID3D11SamplerState;

enum class ConstantBuffer {
	Camera,
	Shadow,
	Material,
	Bones,
	FXAA,
	MSAA
};

enum class Pattern {
	Model,
	Wall,
	Shadow,
	FXAA,
	SMAA,
};

template <typename T, typename U>
using ResourceMap = std::unordered_map<T, Microsoft::WRL::ComPtr<U>>;

struct CommonResource {
	ResourceMap<Pattern, ID3D11VertexShader> vertex_shaders;
	ResourceMap<Pattern, ID3D11PixelShader> pixel_shaders;
	ResourceMap<ConstantBuffer, ID3D11Buffer> constant_buffers;
	ResourceMap<Pattern, ID3D11InputLayout> input_layouts;
	ResourceMap<Pattern, ID3D11ShaderResourceView> shader_resouce_view;
	ResourceMap<Pattern, ID3D11DepthStencilState> depth_stencil_state;
	ResourceMap<Pattern, ID3D11DepthStencilView> depth_stencil_view;
	ResourceMap<Pattern, ID3D11SamplerState> sampler_state;
};
