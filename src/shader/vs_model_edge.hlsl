#include "common/input_vs_model.hlsl"
#include "common/input_ps_model_edge.hlsl"
#include "common/constant_buffer/model_edge.hlsl"
#include "common/constant_buffer/camera.hlsl"
#include "common/constant_buffer/bones.hlsl"

PSInput main(const VSInput input) {
    PSInput output;

    // スキニング
    const float4 expanded_position = float4(input.position + input.normal * edge_width, 1.0f);
    const float4 skinned_pos = skinned_position(expanded_position, input.bones, input.weights);
    
    // カメラ
    const float4 world_pos = mul(skinned_pos, world);
    const float4 view_pos = mul(world_pos, view);
    const float4 clip_pos = mul(view_pos, proj);
    
    output.position = clip_pos;
 
    return output;
}
