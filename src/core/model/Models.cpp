#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Models.h"
#include "pmd/PMDModel.h"

constexpr char RESOURCE_FILE_NAME[] = "assets/model_list.txt";

using model_list = std::vector<std::filesystem::path>;
std::optional<model_list> get_model_files(const char* resouce_file_path);

std::unique_ptr<IModel> load_pmd(const std::filesystem::path& path) {
	return std::make_unique<PMDModel>(path);
}

Models::Models(void) {
	this->support_extensions.emplace(".pmd", load_pmd);
}

bool Models::init() {
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

		this->models.emplace(file_name, result->second(model_file));
		this->name_map.emplace(file_name, model_file.u8string());
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

bool Models::add_model(const std::unique_ptr<IModel>& model) {
	if(!model->init()) {
		return false;
	}

	return false;
}

void Models::set_model(const std::u8string& model_name) {
	this->current_model = model_name;
}

const IModel* Models::get_current_model(void) const {
	return this->models.at(this->current_model).get();
}

void Models::load_current_model(ID3D11Device* const device) {
	const auto& path = this->name_map.at(this->current_model);
	this->models.at(this->current_model)->load_model(path, device);
}

void Models::unload_model(const std::u8string& model_name) {
	this->models.at(model_name)->unload_model();
}

void Models::update(ID3D11DeviceContext* const context) {
	this->models.at(this->current_model)->update(context);
}

void Models::render(ID3D11DeviceContext* const context) const {
	this->models.at(this->current_model)->render(context);
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

	return std::optional<model_list>(list);
}