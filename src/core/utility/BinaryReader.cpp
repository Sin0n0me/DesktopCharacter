#include <filesystem>
#include "BinaryReader.h"

bool BinaryReader::read(void* data, const int size) {
	if(this->file.bad()) {
		return false;
	}

	this->file.read(
		reinterpret_cast<char*>(data),
		size
	);

	if(this->file.fail()) {
		return false;
	}

	if(this->file.gcount() != size) {
		return false;
	}

	return true;
}

std::vector<uint8_t> BinaryReader::read_all(void) {
	// 一旦終端まで移動しtellgで現在位置からサイズを求める
	this->file.seekg(0, std::ios::end);
	const std::streamsize size = this->file.tellg();
	this->file.seekg(0, std::ios::beg);

	std::vector<uint8_t> vec;
	this->read_to_vec(vec, size);
	return vec;
}

std::optional<BinaryReader> BinaryReader::make_reader(const std::filesystem::path& path) {
	BinaryReader reader{};

	// 念のためファイルが存在するかチェック
	if(!std::filesystem::exists(path)) {
		return std::optional<BinaryReader>();
	}

	reader.file = std::ifstream(path, std::ios::binary | std::ios::in);
	if(!reader.file.is_open()) {
		return std::optional<BinaryReader>();
	}

	return reader;
}