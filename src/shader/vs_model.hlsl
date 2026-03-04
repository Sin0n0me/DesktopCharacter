#include "common/input_vs_model.hlsl"
#include "common/input_ps_model.hlsl"
#include "common/constant_buffer/camera.hlsl"
#include "common/constant_buffer/bones.hlsl"

PSInput main(const VSInput input) {
    PSInput output;
    const float4 local_pos = float4(input.position, 1.0f);
        
    // スキニング
    const float4 skinned_pos = skinned_position(local_pos, input.bones, input.weights);
    const float4 clip_pos = apply_camera(skinned_pos);
    const float3 skinned_nor = skinned_normal(input.normal, input.bones, input.weights);
    
    output.position = clip_pos;
    output.normal = skinned_nor;
    output.uv = input.uv;
    
    return output;
}
