#pragma once
#include <fstream>
#include <optional>
#include <vector>

class BinaryReader {
private:
    std::ifstream file;

public:

    static std::optional<BinaryReader> make_reader(const std::filesystem::path& path);

    bool read(void* data, const int size);

    template<typename T>
    bool read_to(T* data) {
        return this->read(data, sizeof(T));
    }

    template<typename T>
    bool read_to_vec(std::vector<T>& data, const uint32_t count) {
        data.resize(count);
        return this->read(data.data(), sizeof(T) * count);
    }

    std::vector<uint8_t> read_all(void);

    template<typename T>
    bool read_magic_number(T* data) {
        this->file.seekg(0, std::ios::beg);

        if(!this->read(data, sizeof(T))) {
            return false;
        }

        this->file.seekg(0, std::ios::beg);

        return true;
    }
};
