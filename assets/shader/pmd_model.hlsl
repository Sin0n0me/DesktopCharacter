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

    /*
    output.shadow = mul(wpos, light_view_proj);
    output.shadow.xyz /= output.shadow.w;
    output.shadow.y *= -1.0f;
    output.shadow.xy = output.shadow.xy * 0.5f + 0.5f;
    */
    
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    const float4 tex = model_texture.Sample(model_sampler, input.uv);
    const float4 base = tex * diffuse;// + float4(ambient, 1);

    /*
    const float depth = input.shadow.z;
    const float shadow = shadow_texture.SampleCmpLevelZero(
        shadow_sampler,
        input.shadow.xy,
        depth
    );
    // ”wŒi‚ªŒ©‚¦‚é‚æ‚¤‚É‚ ‚é’ö“x“§‰ß‚³‚¹‚é
    const float alpha = (1.0f - shadow) * 0.5;
    */
    
    return base;
}
