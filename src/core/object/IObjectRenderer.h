#pragma once

struct ID3D11DeviceContext;
struct ID3D11Device;

class IObjectRenderer {
public:
	virtual ~IObjectRenderer(void) = default;

	virtual bool init(ID3D11Device* const device) = 0;

	virtual void update(ID3D11DeviceContext* const context) = 0;

	virtual void render(ID3D11DeviceContext* const context) const = 0;
};
