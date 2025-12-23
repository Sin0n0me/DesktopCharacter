#pragma once
#include <d3d11.h>

class IModelRenderer {
public:
	~IModelRenderer() = default;

	virtual void update(ID3D11DeviceContext* const context) = 0;

	virtual void render(ID3D11DeviceContext* const context) const = 0;
};
