#pragma once
#include "../../constant_buffer/Camera.h"
#include <memory>

struct ID3D11Device;
struct CommonResource;

class SceneCamera {
private:
	Camera camera;
	std::shared_ptr<CommonResource> common_resource;

public:

	static SceneCamera make_camera(ID3D11Device* const device, const std::shared_ptr<CommonResource>& common_resource);
};
