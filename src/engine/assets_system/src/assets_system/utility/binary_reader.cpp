#include "binary_reader.h"
#include <expected>
#include <filesystem>
#include <format>
#include <type_traits>

namespace enishi::assets_system {
    IOReuslt<BinaryReader> BinaryReader::make_reader(const std::filesystem::path& path) {
        BinaryReader reader{};

        // 念のためファイルが存在するかチェック
        if (!std::filesystem::exists(path)) {
            return foundation::Error(
                IOError::FileNotFound, path.string() + " というパスもしくはファイルは存在しません");
        }

        reader.file = std::ifstream(path, std::ios::binary | std::ios::in);
        if (!reader.file.is_open()) {
            return foundation::Error(IOError::PermissionDenied, "ファイルが開けませんでした");
        }

        return std::move(reader);
    }

    IOReuslt<void> BinaryReader::read(void* data, const int size) {
        if (this->file.bad()) {
            return foundation::Error(IOError::BrokenStream, "streamが壊れています");
        }

        this->file.read(reinterpret_cast<char*>(data), size);

        if (this->file.fail()) {
            return foundation::Error(IOError::ReadFailed, "ファイルの読み取りに失敗しました");
        }

        const auto read_size = this->file.gcount();
        if (read_size != size) {
            std::string mes = std::format(
                "正常に読み取りができませんでした\n期待したサイズ: {}\n実際に読み取ったサイズ: {}",
                size,
                read_size);
            return foundation::Error(IOError::InvalidData, std::string(mes.begin(), mes.end()));
        }

        return {};
    }

    IOReuslt<std::vector<std::uint8_t>> BinaryReader::read_all(void) {
        // 一旦終端まで移動しtellgで現在位置からサイズを求める
        this->file.seekg(0, std::ios::end);
        const std::streamsize size = this->file.tellg();
        this->file.seekg(0, std::ios::beg);

        std::vector<std::uint8_t> vec;

        const auto result = this->read_to_vec(vec, size);
        if (!result.has_value()) {
            return std::unexpected(result.error());
        }

        return vec;
    }

    IOReuslt<void> BinaryReader::read_magic_number(const std::string& expect) {
        // 現在の読み込み位置を保存
        const std::streampos saved_pos = file.tellg();
        if (saved_pos == std::streampos(-1)) {
            return foundation::Error(
                IOError::InvalidStreamPosition, "Streamが無効な位置を指しています");
        }

        // 先頭に戻す
        this->file.seekg(0, std::ios::beg);

        // 比較用に一時バッファ作成
        const std::uint32_t size = expect.size();
        std::unique_ptr<char[]> buff = std::make_unique<char[]>(size);
        const auto result = this->read(buff.get(), sizeof(char) * size);
        if (!result.has_value()) {
            return result;
        }

        // 読み進めた分をもとに戻す
        this->file.seekg(saved_pos);

        // マジックナンバーの比較
        if (std::strncmp(buff.get(), expect.c_str(), size) != 0) {
            std::string mes = std::format(
                "予期しないファイルヘッダです\n期待した値: {}\n実際の値: {}", expect, size);
            return foundation::Error(IOError::MismatchHeader, std::string(mes.begin(), mes.end()));
        }

        return {};
    }
} // namespace enishi::assets_system