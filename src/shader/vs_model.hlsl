#include "common/vs_input_model.hlsl"
#include "common/ps_input_model.hlsl"
#include "common/constant_buffer/camera.hlsl"
#include "common/constant_buffer/bones.hlsl"

PSInput main(const VSInput input) {
    PSInput output;
    
    // スキニング
    const float4 local_pos = float4(input.position, 1.0f);
    const float3 local_nor = input.normal;
    
    const uint x_index = input.bones & 0x0000FFFFu;
    const uint y_index = (input.bones & 0xFFFF0000u) >> 16;
    
    const float4 bone_pos_x = mul(local_pos, bone_matrices[x_index]) * input.weights.x;
    const float4 bone_pos_y = mul(local_pos, bone_matrices[y_index]) * input.weights.y;
    const float4 skinned_pos = bone_pos_x + bone_pos_y;

    const float3 nor_x = mul(local_nor, (float3x3)bone_matrices[x_index]) * input.weights.x;
    const float3 nor_y = mul(local_nor, (float3x3)bone_matrices[y_index]) * input.weights.y;
    const float3 skinned_nor = nor_x + nor_y;

    //skinned_nor = normalize(skinned_nor);
    
    // カメラ
    const float4 world_pos = mul(skinned_pos, world);
    const float4 view_pos = mul(world_pos, view);
    const float4 clip_pos = mul(view_pos, proj);

    output.position = clip_pos;
    output.uv = input.uv;

    return output;
}
