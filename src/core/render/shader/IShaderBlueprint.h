#pragma once
#include <d3dcommon.h>
struct ID3D11Device;
struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

class IShaderBlueprint {
public:
	virtual ~IShaderBlueprint(void) = default;

	virtual bool make_vertex_shader(
		ID3D11Device* const device,
		ID3D11VertexShader** vertex_shader,
		ID3DBlob** vs_blob,
		ID3DBlob** error_blob
	) = 0;

	virtual bool make_pixel_shader(
		ID3D11Device* const device,
		ID3D11PixelShader** pixel_shader,
		ID3DBlob** ps_blob,
		ID3DBlob** error_blob
	) = 0;

	virtual bool make_input_layout(
		ID3D11Device* const device,
		ID3D11InputLayout** input_layout,
		ID3DBlob** vs_blob
	) = 0;
};
