#pragma once

struct alignas(16) Material {
    float diffuse[4];
    float specular[3];
    float shininess;
    float ambient[3];
    float sphere_mul; // ifによる分岐を減らすため ０-1 で計算
    float sphere_add;
    float edge_flag;
    float _pad[2];
};
