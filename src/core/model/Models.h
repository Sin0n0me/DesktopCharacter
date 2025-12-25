#pragma once

#include "PMDModel.h"

class Models : public IModelRenderer {
private:
	std::unordered_map<std::u8string, PMDModel> models;
	std::u8string current_model;

public:
	Models(void) = default;

	bool load_models();

	void set_current_model(ID3D11Device* const device);

	void update(ID3D11DeviceContext* const context) override;

	void render(ID3D11DeviceContext* const context) const override;
};
