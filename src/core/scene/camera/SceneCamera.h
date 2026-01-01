#pragma once
#include "../../render/constant_buffer/Camera.h"
#include <memory>

struct ID3D11Device;
struct CommonResource;

struct SceneCamera {
	Camera camera;

	static SceneCamera make_camera(ID3D11Device* const device, const std::shared_ptr<CommonResource>& common_resource);
};
