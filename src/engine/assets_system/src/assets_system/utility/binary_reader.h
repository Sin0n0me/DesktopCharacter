#pragma once
#include "../errors/errors.h"
#include <filesystem>
#include <fstream>
#include <stdint.h>
#include <string>
#include <vector>

namespace enishi::assets_system {
    class BinaryReader {
      private:
        std::ifstream file;

      public:
        [[nodiscard]] static IOReuslt<BinaryReader> make_reader(const std::filesystem::path& path);

        [[nodiscard]] IOReuslt<void> read(void* data, const int size);

        template <typename T> [[nodiscard]] IOReuslt<void> read_to(T* data) {
            return this->read(data, sizeof(T));
        }

        template <typename T>
        [[nodiscard]] IOReuslt<void> read_to_vec(std::vector<T>& data, const std::uint32_t count) {
            data.resize(count);
            return this->read(data.data(), sizeof(T) * count);
        }

        [[nodiscard]] IOReuslt<std::vector<std::uint8_t>> read_all(void);

        [[nodiscard]] IOReuslt<void> read_magic_number_from_str(const std::string& expect);

        template <typename T> [[nodiscard]] IOReuslt<void> read_magic_number(const T expect) {
            // 現在の読み込み位置を保存
            const std::streampos saved_pos = file.tellg();
            if (saved_pos == std::streampos(-1)) {
                return foundation::Error(
                    IOError::InvalidStreamPosition, "Streamが無効な位置を指しています");
            }

            // 先頭に戻す
            this->file.seekg(0, std::ios::beg);

            T found{};
            const auto result = this->read_to(&found);
            if (!result.has_value()) {
                return result;
            }

            // 読み進めた分をもとに戻す
            this->file.seekg(saved_pos);

            // マジックナンバーの比較
            if (found == expect) {
                return foundation::Error(IOError::MismatchHeader,
                    std::format("mismatch header. expect: {} found: {}", expect, found));
            }

            return {};
        }
    };
} // namespace enishi::assets_system
