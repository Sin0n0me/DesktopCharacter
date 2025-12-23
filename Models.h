#pragma once
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <optional>
#include "PMDModel.h"

using model_list = std::vector<std::filesystem::path>;

class Models : public IModelRenderer {
private:
	std::unordered_map<std::u8string, PMDModel> models;
	std::u8string current_model;

	Models(void) = default;
public:
	static std::optional<Models> load_models();

	void set_current_model(ID3D11Device* const device);

	void update(ID3D11DeviceContext* const context) override;

	void render(ID3D11DeviceContext* const context) const override;
};
