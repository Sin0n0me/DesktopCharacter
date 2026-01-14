#include "../CommonResource.h"
#include "ModelManager.h"
#include "pmd/PMDModel.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

constexpr char RESOURCE_FILE_NAME[] = "model_list.txt";

using model_list = std::vector<std::filesystem::path>;
std::optional<model_list> get_model_files(const char* resouce_file_path);

static std::shared_ptr<Model> load_pmd(const std::filesystem::path& path) {
    return std::make_shared<PMDModel>(path);
}

ModelManager::ModelManager(const std::shared_ptr<CommonResource>& common_resource) {
    this->resource = common_resource;
    this->support_extensions.emplace(".pmd", load_pmd);
}

bool ModelManager::init(void) {
    // まずはモデルの有無判定
    const auto model_files = get_model_files(RESOURCE_FILE_NAME);
    if(!model_files.has_value()) {
        return false;
    }

    // モデルが1つもない
    if(model_files.value().empty()) {
        return false;
    }

    // モデルの仮読み込み & 登録
    for(const auto& model_file : model_files.value()) {
        // 拡張子なしは判断不能
        if(!model_file.has_extension()) {
            return false;
        }

        // 対応している拡張子に応じたモデルクラス作成
        const auto& extension = model_file.extension().string();
        const auto& file_name = model_file.filename().u8string();
        const auto& result = this->support_extensions.find(extension);
        if(result == this->support_extensions.end()) {
            return false;
        }
        const auto& model_loader = result->second;

        // モデルの追加
        if(!this->add_model(model_loader(model_file))) {
            return false;
        }
    }

    // 最初に見つかったモデルを使用
    // TODO: 任意のモデルへ変更可能にする
    for(const auto& i : this->models) {
        this->current_model = i.first;
        break;
    }

    if(this->models.empty()) {
        return false;
    }

    return true;
}

bool ModelManager::add_model(std::shared_ptr<Model> model) {
    const auto& model_name = model->get_file_name();
    this->model_obb_map.emplace(model_name, OBBMap{});
    this->models[model_name] = std::move(model);

    return true;
}

void ModelManager::set_model(const std::u8string& model_name) {
    this->current_model = model_name;
}

const std::shared_ptr<Model> ModelManager::get_current_model(void) const {
    return this->models.at(this->current_model);
}

const OBBMap& ModelManager::get_obb_map(void) const {
    return this->model_obb_map.at(this->current_model);
}

bool ModelManager::load_current_model(ID3D11Device* const device) {
    const auto& model_name = this->current_model;
    const auto& model = this->models.at(model_name);

    if(!model->init(device)) {
        return false;
    }

    model->compute_obb(this->model_obb_map.at(model_name));

    return true;
}

void ModelManager::unload_model(const std::u8string& model_name) {
    this->models.at(model_name)->unload_model();
}

void ModelManager::update(const int64_t delta_time) {
    const auto& model = this->models.at(this->current_model);
    model->update_motion(delta_time);
    model->update_obb(this->model_obb_map.at(this->current_model));
}

void ModelManager::update_render(ID3D11DeviceContext* const context) {
    const auto& model = this->models.at(this->current_model);
    model->render_update(context);
}

// 拡張子付きのファイルであればモデルファイルとみなす
std::optional<model_list> get_model_files(const char* resouce_file_path) {
    std::ifstream ifs(resouce_file_path); // ファイルを開く
    std::string line;

    if(!ifs.is_open()) {
        std::cerr << "can not open file: " << resouce_file_path << std::endl;
        return std::optional<model_list>();
    }

    model_list list{};
    while(std::getline(ifs, line)) {
        if(line.empty()) {
            continue;
        }

        const std::u8string line_u8(line.begin(), line.end());
        const std::u8string full_path = std::filesystem::current_path().u8string() + u8"/" + line_u8;

        const auto path = std::filesystem::path(full_path).lexically_normal();
        if(path.empty()) {
            continue;
        }
        if(!path.has_extension()) {
            continue;
        }

        list.emplace_back(path);
    }

    ifs.close(); // ファイルを閉じる

    return list;
}