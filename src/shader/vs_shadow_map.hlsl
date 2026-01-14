#include "common/input_vs_model.hlsl"
#include "common/constant_buffer/camera.hlsl"
#include "common/constant_buffer/bones.hlsl"
#include "common/constant_buffer/shadow_map.hlsl"

struct VSOutput {
    float4 position : SV_POSITION;
};

VSOutput main(const VSInput input) {
    VSOutput output;
    const float4 local_pos = float4(input.position, 1.0f);
    const float4 skinned_pos = skinned_position(local_pos, input.bones, input.weights);
    output.position = mul(skinned_pos, world);
    output.position = mul(output.position, light_view_proj);    
    return output;
}
