#include "Models.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

constexpr char RESOURCE_FILE_NAME[] = "assets/model_list.txt";

using model_list = std::vector<std::filesystem::path>;
std::optional<model_list> get_model_files(const char* resouce_file_path);

bool Models::load_models() {
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
		const auto& model = PMDModel::load_pmd(model_file);
		if(!model.has_value()) {
			// TODO: add log
			continue;
		}

		this->models.emplace(model_file.u8string(), model.value());
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

void Models::set_current_model(ID3D11Device* const device) {
	this->models.at(this->current_model).make_buffers(device);
}

void Models::update(ID3D11DeviceContext* const context) {
	this->models.at(this->current_model).update(context);
}

void Models::render(ID3D11DeviceContext* const context) const {
	this->models.at(this->current_model).render(context);
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