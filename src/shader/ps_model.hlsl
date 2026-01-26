#include "common/input_ps_model.hlsl"
#include "common/constant_buffer/material.hlsl"

Texture2D model_texture;
Texture2D sphere_texture;
Texture2D toon_texture;
SamplerState model_sampler;
SamplerState sphere_sampler;
SamplerState toon_sampler;


float4 main(const PSInput input) : SV_TARGET {
    const float4 base_color = model_texture.Sample(model_sampler, input.uv);
    
    // TODO: 光源位置や色を定数バッファで指定できるように
    const float3 light_direction = normalize(
        float3(20.0f, 20.0f, -30.0f)
    ); 
    const float4 light_color = float4(1.0, 1.0, 1.0, 1.0);
    const float4 lighting_color = light_color * apply_lighting(
        base_color,
        input.position.xyz, 
        light_direction, 
        input.normal
    );
    
    const float dot_nl = saturate(dot(normalize(input.normal), light_direction));
    const float2 toon_uv = float2(dot_nl, 0.0f);
    const float3 toon_color = toon_texture.Sample(toon_sampler, toon_uv).rgb;
    const float4 apply_toon_color = lighting_color; // * float4(toon_color, 1.0);
    
    const float2 sphere_uv = calc_sphere_uv(input.position.xyz, input.normal);
    const float4 sphere_color = sphere_texture.Sample(sphere_sampler, sphere_uv);
    
    const float4 final_color = apply_sphere_map_branchless(
        apply_toon_color,
        sphere_color
    );
    
    return final_color;
}
