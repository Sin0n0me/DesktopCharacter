#pragma once
#include "OBB.h"
#include "Ray.h"

class Collider {
private:

public:

	bool hit_model(const Ray& ray, const OBB& obb);
};
