#pragma once
#include "camera/SceneCamera.h"
#include "light/Light.h"

class Scene {
private:
	SceneCamera camera;
	Light light;

public:

	Scene(ID3D11Device* const device, const std::shared_ptr<CommonResource>& common_resouce);

	void update(void);

	void render(void) const;
};
