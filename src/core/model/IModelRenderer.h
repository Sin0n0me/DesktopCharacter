#pragma once

struct ID3D11DeviceContext;

class IModelRenderer {
public:
	virtual ~IModelRenderer() = default;

	virtual void update(ID3D11DeviceContext* const context) = 0;

	virtual void render(ID3D11DeviceContext* const context) const = 0;
};
