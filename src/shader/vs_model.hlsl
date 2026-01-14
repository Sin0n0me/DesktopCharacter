#include "common/input_vs_model.hlsl"
#include "common/input_ps_model.hlsl"
#include "common/constant_buffer/camera.hlsl"
#include "common/constant_buffer/bones.hlsl"

PSInput main(const VSInput input) {
    PSInput output;
    
    // スキニング
    const float4 local_pos = float4(input.position, 1.0f);
    const float4 skinned_pos = skinned_position(local_pos, input.bones, input.weights);
    const float3 skinned_nor = skinned_normal(input.normal,input.bones, input.weights);
    
    // カメラ
    const float4 world_pos = mul(skinned_pos, world);
    const float4 view_pos = mul(world_pos, view);
    const float4 clip_pos = mul(view_pos, proj);

    output.normal = skinned_nor;
    output.position = clip_pos;
    output.uv = input.uv;
    
    return output;
}
