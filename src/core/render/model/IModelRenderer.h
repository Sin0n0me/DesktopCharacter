#pragma once
#include "../../object/IObjectRenderer.h"

class IModelRenderer : public IObjectRenderer {
public:
	virtual ~IModelRenderer(void) noexcept = default;

	virtual void unload_model(void) = 0;

	virtual bool is_loaded_model(void) = 0;
};
