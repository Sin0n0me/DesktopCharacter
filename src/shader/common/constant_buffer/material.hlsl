cbuffer Material {
    float4 diffuse;
    float3 ambient;
    float sphere_mul; // ifによる分岐を減らすため ０-1 で計算
    float sphere_add;
};
