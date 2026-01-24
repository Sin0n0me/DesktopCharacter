#pragma once
#include "OBB.h"

class IModelOBB {
public:
    virtual ~IModelOBB(void) = default;

    virtual void compute_obb(OBBMap& obb_map) = 0;

    virtual void update_obb(OBBMap& obb_map) = 0;
};
