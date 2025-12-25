#pragma once
#include "../../constant_buffer/Shadow.h"
#include <memory>

struct ID3D11Device;
struct CommonResource;

class Light {
private:
	Shadow shadow;
	std::shared_ptr<CommonResource> common_resource;

public:

	static Light make_light(ID3D11Device* const device, const std::shared_ptr<CommonResource>& common_resource);
};
