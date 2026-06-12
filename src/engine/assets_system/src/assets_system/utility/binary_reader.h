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

        [[nodiscard]] IOReuslt<void> read_magic_number(const std::string& expect);
    };
} // namespace enishi::assets_system
