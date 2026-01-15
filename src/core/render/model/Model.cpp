#include "Model.h"

const std::u8string Model::get_file_name(void) const {
    return this->path.filename().u8string();
}