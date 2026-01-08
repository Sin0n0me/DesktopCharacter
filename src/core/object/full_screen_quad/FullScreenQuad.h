#pragma once
#include "../IObjectRenderer.h"
#include <wrl/client.h>

struct ID3D11Buffer;

class FullScreenQuad : public IObjectRenderer {
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

public:

	bool init(ID3D11Device* const device) override;
	void update(ID3D11DeviceContext* const context) override;
	void render(ID3D11DeviceContext* const context) const override;
};
