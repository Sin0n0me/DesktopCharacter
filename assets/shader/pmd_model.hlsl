cbuffer Camera : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
}

cbuffer Material : register(b1)
{
    float4 gDiffuse;
    float3 gAmbient;
};

/*
cbuffer Bones : register(b2) {
    matrix gBones[128];
};
*/

Texture2D gTex : register(t0);
SamplerState gSmp : register(s0);

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

PSInput VSMain(VSInput input)
{
    PSInput output;

    float4 wpos = mul(float4(input.position, 1.0f), world);
    float4 vpos = mul(wpos, view);
    output.position  = mul(vpos, proj);
    output.uv = input.uv;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return gTex.Sample(gSmp, input.uv);
    float4 tex = gTex.Sample(gSmp, input.uv);
    return tex * gDiffuse + float4(gAmbient, 1);
}
