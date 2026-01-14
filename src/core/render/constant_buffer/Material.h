#pragma once

struct alignas(16) Material {
    float diffuse[4];
    float ambient[3];
    float sphere_mul; // ifによる分岐を減らすため ０-1 で計算
    float sphere_add;
};
