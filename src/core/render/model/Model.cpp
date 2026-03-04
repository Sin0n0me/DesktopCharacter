#include "Model.h"
#include <d3d11.h>

Model::Model(const std::filesystem::path& path) :
    path(path) {
}

const std::u8string Model::get_file_name(void) const {
    return this->path.filename().u8string();
}