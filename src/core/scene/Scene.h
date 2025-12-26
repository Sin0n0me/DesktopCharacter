#pragma once
#include "camera/SceneCamera.h"
#include "light/Light.h"

struct ID3D11DeviceContext;

class Scene {
private:
	SceneCamera camera;
	Light light;

	std::shared_ptr<CommonResource> resource;

public:

	Scene(ID3D11Device* const device, const std::shared_ptr<CommonResource>& resouce);

	void update(void);

	void render_update(ID3D11DeviceContext* const context);

	void render(ID3D11DeviceContext* const context) const;
};
