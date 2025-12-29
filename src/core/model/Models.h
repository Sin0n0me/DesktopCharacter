#pragma once

#include "IModel.h"
#include <memory>
#include <string>
#include <unordered_map>

class Models {
private:
	std::unordered_map<std::u8string, std::unique_ptr<IModel>> models; // firlst: name second: model
	std::unordered_map<std::u8string, std::u8string> name_map; // firlst: name second: path
	std::u8string current_model;
	std::unordered_map<std::string, std::unique_ptr<IModel>(*)(const std::filesystem::path&)> support_extensions;

public:
	Models(void);

	bool init();
	bool add_model(const std::unique_ptr<IModel>& model);

	void set_model(const std::u8string& model_name);
	const IModel* get_current_model(void) const;

	void load_current_model(ID3D11Device* const device);
	void unload_model(const std::u8string& model_name);

	void update(ID3D11DeviceContext* const context);
	void render(ID3D11DeviceContext* const context) const;
};
