cbuffer Bones {
    matrix bone_matrices[256];
};


float4 skinned_position(const float4 local_pos, const uint bones, const float2 weights) {
    const uint x_index = bones & 0x0000FFFFu;
    const uint y_index = (bones & 0xFFFF0000u) >> 16;
    const float4 bone_pos_x = mul(local_pos, bone_matrices[x_index]) * weights.x;
    const float4 bone_pos_y = mul(local_pos, bone_matrices[y_index]) * weights.y;
    const float4 skinned_pos = bone_pos_x + bone_pos_y;
    return skinned_pos;
}

float3 skinned_normal(const float3 normal, const uint bones, const float2 weights) {
    const uint x_index = bones & 0x0000FFFFu;
    const uint y_index = (bones & 0xFFFF0000u) >> 16;
    const float3 local_nor = normal;
    const float3 nor_x = mul(local_nor, (float3x3)bone_matrices[x_index]) * weights.x;
    const float3 nor_y = mul(local_nor, (float3x3)bone_matrices[y_index]) * weights.y;
    const float3 skinned_nor = nor_x + nor_y;
    return normalize(skinned_nor);
}

