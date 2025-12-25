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

/*
cbuffer Bones : register(b3) {
    matrix gBones[128];
};
*/

cbuffer Shadow : register(b2) {
    matrix light_view_proj;
};

Texture2D model_texture : register(t0);
Texture2D shadow_texture : register(t1);
SamplerState model_sampler : register(s0);
SamplerComparisonState shadow_map : register(s1);

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

    float4 wpos = mul(float4(input.position, 1.0f), world);
    float4 vpos = mul(wpos, view);
    output.position = mul(vpos, proj);
    output.uv = input.uv;
    output.shadow = mul(float4(input.position,1.0f), light_view_proj);
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 tex = model_texture.Sample(model_sampler, input.uv);
    float4 base = tex * diffuse + float4(ambient, 1);
    float3 projection = input.shadow.xyz / input.shadow.w;
    float2 uv = projection.xy * 0.5f + 0.5f;
    float depth = projection.z;
    float shadow = shadow_texture.SampleCmpLevelZero(shadow_map, uv, depth);
    
    return tex;
    
    float depth2 = shadow_texture.Load(int3(input.position.xy, 0)).r;
    return float4(depth2, depth2, depth2, 1);
    
    return base * shadow;
}
