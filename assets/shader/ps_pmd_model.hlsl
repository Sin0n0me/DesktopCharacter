cbuffer Material : register(b1)
{
    float4 diffuse;
    float3 ambient;
};

/*
cbuffer Bones : register(b2) {
    matrix gBones[128];
};
*/

Texture2D texture_2D : register(t0);
SamplerState gSmp : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};


float4 PSMain(PSInput input) : SV_TARGET
{
    return texture_2D.Sample(gSmp, input.uv);
    float4 tex = texture_2D.Sample(gSmp, input.uv);
    return tex * diffuse + float4(ambient, 1);
}
