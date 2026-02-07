#pragma once
#include "../PMDFileStruct.h"
#include <vector>

struct PMDMorphData {
    uint8_t skin_type;		               // 表情の種類
    std::vector<PMDMorphVertex> vertices;  //
};
