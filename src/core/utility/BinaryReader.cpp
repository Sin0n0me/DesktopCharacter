#include "../log/Logger.h"
#include "BinaryReader.h"
#include <filesystem>

bool BinaryReader::read(void* data, const int size) {
    if(this->file.bad()) {
        return false;
    }

    this->file.read(
        reinterpret_cast<char*>(data),
        size
    );

    if(this->file.fail()) {
        Logger::error(u8"ファイルの読み取りに失敗しました");
        return false;
    }

    if(this->file.gcount() != size) {
        Logger::error(
            Logger::make_message(
                u8"正常に読み取りができませんでした\n",
                std::u8string(u8"期待したサイズ: "),
                size,
                u8"実際に読み取ったサイズ: ",
                this->file.gcount()
            )
        );

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

bool BinaryReader::is_eof(void) const {
    return this->file.eof();
}

std::optional<BinaryReader> BinaryReader::make_reader(const std::filesystem::path& path) {
    BinaryReader reader{};

    // 念のためファイルが存在するかチェック
    if(!std::filesystem::exists(path)) {
        Logger::error(
            path.u8string() +
            u8" というパスもしくはファイルは存在しません"
        );
        return std::optional<BinaryReader>();
    }

    reader.file = std::ifstream(path, std::ios::binary | std::ios::in);
    if(!reader.file.is_open()) {
        Logger::error(u8"ファイルが開けませんでした");
        return std::optional<BinaryReader>();
    }

    Logger::debug(
        u8"open file: " +
        path.u8string()
    );

    return std::move(reader);
}