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
    const float4 clip_pos = apply_camera(skinned_pos);

    output.position = clip_pos;
    output.edge_flag = input.edge_flag;
 
    return output;
}
