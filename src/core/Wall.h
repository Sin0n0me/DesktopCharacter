#pragma once
#include "model/IModelRenderer.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <cstdint>

class Wall : public IModelRenderer {
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	uint32_t out_index_count;

	Wall() = default;

public:

	static void create_shadow_receiver_mesh(ID3D11Device* const device);

	void update(ID3D11DeviceContext* const context) override;

	void render(ID3D11DeviceContext* const context) const override;
};
