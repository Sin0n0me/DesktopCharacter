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

cbuffer Bones : register(b3) {
    matrix bone_matrices[256];
};

Texture2D model_texture : register(t0);
Texture2D shadow_texture : register(t1);
SamplerState model_sampler : register(s0);
SamplerComparisonState shadow_sampler : register(s1);

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    uint2 bones : BONEINDICES;
    float2 weights : BONEWEIGHTS;
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
    
    // スキニング
    const float4 local_pos = float4(input.position, 1.0f);
    const float3 local_nor = input.normal;

    const float4 bone_pos_x = mul(local_pos, bone_matrices[input.bones.x]) * input.weights.x;
    const float4 bone_pos_y = mul(local_pos, bone_matrices[input.bones.y]) * input.weights.y;
    const float4 skinned_pos = bone_pos_x + bone_pos_y;

    const float3 nor_x = mul(local_nor, (float3x3)bone_matrices[input.bones.x]) * input.weights.x;
    const float3 nor_y = mul(local_nor, (float3x3)bone_matrices[input.bones.y]) * input.weights.y;
    const float3 skinned_nor = nor_x + nor_y;

    //skinned_nor = normalize(skinned_nor);

    
    // カメラ
    const float4 world_pos = mul(skinned_pos, world);
    const float4 view_pos = mul(world_pos, view);
    const float4 clip_pos = mul(view_pos, proj);

    output.position = clip_pos;    
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
    const float4 base = tex * diffuse;//+ float4(ambient, 1);

    /*
    const float depth = input.shadow.z;
    const float shadow = shadow_texture.SampleCmpLevelZero(
        shadow_sampler,
        input.shadow.xy,
        depth
    );
    // 背景が見えるようにある程度透過させる
    const float alpha = (1.0f - shadow) * 0.5;
    */
    
    return base;
}
