#pragma once
#include "OBB.h"
#include <unordered_map>

using OBBMap = std::unordered_map<short, OBB>;

class IModelOBB {
public:
    virtual ~IModelOBB(void) = default;

    virtual void compute_obb(OBBMap& obb_map) = 0;

    virtual void update_obb(OBBMap& obb_map) = 0;
};
