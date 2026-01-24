#pragma once
#include "OBB.h"

class IOBBMapGetter {
public:
    virtual ~IOBBMapGetter(void) noexcept = default;

    virtual const OBBMap& get_obb_map(void) const = 0;
};
