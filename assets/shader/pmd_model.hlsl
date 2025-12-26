cbuffer Camera : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
}

cbuffer Material : register(b1)
{
    float4 diffuse;
    float3 ambient;
};

cbuffer Shadow : register(b2) {
    matrix light_view_proj;
};

/*
cbuffer Bones : register(b3) {
    matrix gBones[128];
};
*/

Texture2D model_texture : register(t0);
Texture2D shadow_texture : register(t1);
SamplerState model_sampler : register(s0);
SamplerComparisonState shadow_sampler : register(s1);

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 shadow : TEXCOORD1;
};

PSInput VSMain(VSInput input)
{
    PSInput output;

    const float4 wpos = mul(float4(input.position, 1.0f), world);
    const float4 vpos = mul(wpos, view);
    output.position = mul(vpos, proj);
    output.uv = input.uv;
    output.shadow = mul(wpos, light_view_proj);
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    const float4 tex = model_texture.Sample(model_sampler, input.uv);
    const float4 base = tex * diffuse;// + float4(ambient, 1);
    const float3 shadow_ndc = input.shadow.xyz / input.shadow.w;
    float2 uv = shadow_ndc.xy * 0.5f + 0.5f;
    uv.y *= -1.0f;
    const float depth = shadow_ndc.z * 0.5f + 0.5f;
    const float shadow = shadow_texture.SampleCmpLevelZero(shadow_sampler, uv, depth);
    
    //return float4(depth, depth, depth, 1);    
    const float4 result = float4(base.xyz * shadow, base.w);
    return result;
}
