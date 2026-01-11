#include "common/ps_input_model.hlsl"
#include "common/constant_buffer/material.hlsl"

Texture2D model_texture : register(t0);
Texture2D sphere_texture : register(t1);
SamplerState model_sampler : register(s0);
SamplerState sphere_sampler : register(s1);

float4 main(PSInput input) : SV_TARGET {
    const float4 tex = model_texture.Sample(model_sampler, input.uv);
    const float4 base = tex * diffuse;//+ float4(ambient, 1);

    return base;
}
