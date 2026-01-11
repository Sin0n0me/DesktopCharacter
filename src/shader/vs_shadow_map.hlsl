#include "common/vs_input_model.hlsl"
#include "common/constant_buffer/camera.hlsl"
#include "common/constant_buffer/bones.hlsl"
#include "common/constant_buffer/shadow_map.hlsl"

struct VSOutput {
    float4 position : SV_POSITION;
};

VSOutput main(VSInput input) {
    VSOutput output;
    const float4 local_pos = float4(input.position, 1.0f);
    const float4 bone_pos_x = mul(local_pos, bone_matrices[input.bones.x]) * input.weights.x;
    const float4 bone_pos_y = mul(local_pos, bone_matrices[input.bones.y]) * input.weights.y;
    const float4 skinned_pos = bone_pos_x + bone_pos_y;
    output.position = mul(skinned_pos, world);
    output.position = mul(output.position, light_view_proj);
    return output;
}
