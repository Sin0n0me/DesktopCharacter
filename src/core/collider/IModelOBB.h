#pragma once
#include <vector>
#include "OBB.h"

class IModelOBB {
public:

	virtual ~IModelOBB(void) = default;

	virtual bool compute_obb(void) = 0;

	virtual void update_obb(void) = 0;

	virtual const std::vector<OBB>& get_obb(void) const = 0;
};
