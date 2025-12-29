#pragma once
#include "../render/IModelRenderer.h"
#include "../collider/IModelOBB.h"

class IModel : public IModelRenderer, public IModelOBB {
public:
	virtual ~IModel(void) = default;
};
