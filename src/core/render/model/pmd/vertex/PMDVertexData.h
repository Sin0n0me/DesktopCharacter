#pragma once
#include <cstdint>

struct PMDVertexData {
    float position[3];
    float normal[3];
    float uv[2];
    uint16_t bone_index[2];	// ボーン番号(GPU上では32bitのuint扱い)
    float bone_weight[2]; // 0-1
};
