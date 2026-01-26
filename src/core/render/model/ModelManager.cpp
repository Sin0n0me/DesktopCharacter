#include "../../log/Logger.h"
#include "../CommonResource.h"
#include "ModelManager.h"
#include "pmd/PMDModel.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

constexpr char RESOURCE_FILE_NAME[] = "model_list.txt";

using model_list = std::vector<std::filesystem::path>;
std::optional<model_list> get_model_files(const std::filesystem::path& resouce_file_path);

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
        Logger::error(u8"モデルのパスが記述されたファイルが見つかりません");
        return false;
    }

    // モデルの仮読み込み & 登録
    for(const auto& model_file : model_files.value()) {
        // 拡張子なしは判断不能
        if(!model_file.has_extension()) {
            Logger::warning(
                Logger::make_message(
                    u8"判別不可能なファイルです\n",
                    u8"読み込みをスキップしました\n",
                    u8"パス: ",
                    model_file.u8string()
                )
            );
            continue;
        }

        // 対応している拡張子に応じたモデルクラス作成
        const auto& extension = model_file.extension().string();
        const auto& file_name = model_file.filename().u8string();
        const auto& result = this->support_extensions.find(extension);
        if(result == this->support_extensions.end()) {
            Logger::warning(
                Logger::make_message(
                    u8"対応していない拡張子です\n",
                    u8"読み込みをスキップしました\n",
                    u8"パス: ",
                    model_file.u8string()
                )
            );
            continue;
        }
        const auto& model_loader = result->second;

        // モデル(モデルのパス情報のみ)の追加
        if(!this->add_model(model_loader(model_file))) {
            Logger::error(
                Logger::make_message(
                    u8"モデルのパス情報追加に失敗しました\n",
                    u8"パス: ",
                    model_file.u8string()
                )
            );
            continue;
        }

        Logger::info(
            Logger::make_message(
                u8"取得したモデルのパス: ",
                model_file.u8string()
            )
        );
    }

    if(this->models.empty()) {
        Logger::error(u8"モデルのパスが1つもありません");
        return false;
    }

    // 最初に見つかったモデルを使用
    // TODO: 任意のモデルへ変更可能にする
    for(const auto& i : this->models) {
        this->current_model = i.first;
        break;
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
        Logger::error(u8"モデルの初期化に失敗しました");
        return false;
    }

    Logger::info(
        Logger::make_message(
            u8"モデルの初期化に成功しました\n",
            u8"モデル名: ",
            model_name
        )
    );

    model->compute_obb(this->model_obb_map.at(model_name));

    return true;
}

void ModelManager::unload_model(const std::u8string& model_name) {
    this->models.at(model_name)->unload_model();
}

void ModelManager::update(const DeltaTime& delta_time) {
    const auto& model = this->models.at(this->current_model);
    model->update_motion(delta_time);
    model->update_obb(this->model_obb_map.at(this->current_model));
}

void ModelManager::update_render(ID3D11DeviceContext* const context) {
    const auto& model = this->models.at(this->current_model);
    model->render_update(context);
}

// 拡張子付きのファイルであればモデルファイルとみなす
std::optional<model_list> get_model_files(const std::filesystem::path& resouce_file_path) {
    std::ifstream ifs(resouce_file_path);
    std::string line;

    if(!ifs.is_open()) {
        Logger::error(
            Logger::make_message(
                u8"データファイルを開けません\nファイル名: ",
                resouce_file_path.u8string()
            )
        );

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

        Logger::debug(
            Logger::make_message(
                u8"found: ",
                full_path
            )
        );

        list.emplace_back(path);
    }

    ifs.close(); // ファイルを閉じる

    return list;
}