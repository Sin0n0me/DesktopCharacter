#pragma once
#include "../PMDFileStruct.h"
#include <vector>

struct PMDMorphData {
    const uint8_t skin_type;		             // 表情の種類
    const std::vector<PMDMorphVertex> vertices;  //
};
