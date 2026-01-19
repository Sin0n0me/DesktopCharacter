#pragma once

#include "../../timer/Timer.h"
#include "Model.h"
#include <memory>
#include <string>
#include <unordered_map>

struct CommonResource;
using ModelLoaderFunc = std::shared_ptr<Model>(*)(const std::filesystem::path&);

class ModelManager {
private:
    std::unordered_map<std::u8string, std::shared_ptr<Model>> models; // firlst: name second: model
    std::u8string current_model;
    std::unordered_map<std::string, ModelLoaderFunc> support_extensions;
    std::unordered_map<std::u8string, OBBMap> model_obb_map;

    std::shared_ptr<CommonResource> resource;

public:
    ModelManager(const std::shared_ptr<CommonResource>& common_resource);

    bool init(void);
    bool add_model(std::shared_ptr<Model> model);

    void set_model(const std::u8string& model_name);
    const std::shared_ptr<Model> get_current_model(void) const;
    const OBBMap& get_obb_map(void) const;

    bool load_current_model(ID3D11Device* const device);
    void unload_model(const std::u8string& model_name);

    void update(const DeltaTime& delta_time);
    void update_render(ID3D11DeviceContext* const context);
};
