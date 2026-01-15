#pragma once
#include "../../render/constant_buffer/Shadow.h"
#include <memory>

struct ID3D11Device;
struct CommonResource;

struct Light {
    Shadow shadow;

    static Light make_light(ID3D11Device* const device, const std::shared_ptr<CommonResource>& common_resource);
};
