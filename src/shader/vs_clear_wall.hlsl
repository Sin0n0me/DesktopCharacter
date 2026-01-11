#include "common/ps_input_clear_wall.hlsl"
#include "common/constant_buffer/camera.hlsl"
#include "common/constant_buffer/shadow_map.hlsl"

struct VSInput {
    float3 position : POSITION;
};

PSInput main(VSInput input) {
    PSInput output;
    
    const float4 wpos = mul(float4(input.position, 1.0f), world);
    const float4 vpos = mul(wpos, view);
    output.position = mul(vpos, proj);
    
    output.shadow = mul(wpos, light_view_proj);
    output.shadow.xyz /= output.shadow.w;
    output.shadow.y *= -1.0f;
    output.shadow.xy = output.shadow.xy * 0.5f + 0.5f;
    
    return output;
}
