#include "common/input_ps_model.hlsl"
#include "common/constant_buffer/material.hlsl"

Texture2D model_texture;
Texture2D sphere_texture;
SamplerState model_sampler;
SamplerState sphere_sampler;

float2 calc_sphere_uv(
    const float3 position_vs,
    const float3 normal_vs
) {
    const float3 view_dir = normalize(-position_vs);
    const float3 reflect_dir = reflect(-view_dir, normalize(normal_vs));
    const float dot_reflect = dot(reflect_dir.xy, reflect_dir.xy);
    const float pow_reflect = (reflect_dir.z + 1.0f) * (reflect_dir.z + 1.0f);
    const float m = 2.0f * sqrt(dot_reflect + pow_reflect);
    const float inv_m = rcp(max(m, 1e-5f));

    return float2(
        reflect_dir.x * inv_m + 0.5f,
        -reflect_dir.y * inv_m + 0.5f
    );
}

// IFによる分岐を減らすために以下のような回りくどい計算をしている
float4 apply_sphere_map_branchless(
    const float4 base_color,
    const float4 sphere_color
) {
    const float4 mul_case = (base_color * sphere_mul) * (sphere_color * sphere_mul);
    const float4 add_case = (base_color * sphere_add) + (sphere_color * sphere_add);
    const float no_case = 1.0 - (sphere_mul + sphere_add); // 適用なしの場合
    const float4 result = base_color * no_case + add_case * sphere_add + mul_case * sphere_mul;
    return result; 
}


float4 main(const PSInput input) : SV_TARGET {
    const float4 sample_color = model_texture.Sample(model_sampler, input.uv);
    const float4 base_color = sample_color * diffuse;
    const float2 sphere_uv = calc_sphere_uv(input.position.xyz, input.normal);
    const float4 sphere_color = sphere_texture.Sample(sphere_sampler, sphere_uv);
    const float4 final_color = apply_sphere_map_branchless(base_color, sphere_color);    
    return final_color;
}
