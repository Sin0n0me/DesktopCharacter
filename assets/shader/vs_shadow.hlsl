cbuffer Camera : register(b0) {
    matrix world;
    matrix view;
    matrix proj;
}

cbuffer Shadow : register(b1)
{
    matrix light_view_proj;
};

cbuffer Bones : register(b3) {
    matrix bone_matrices[256];
};

struct VSInput {
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    uint2 bones : BONEINDICES;
    float2 weights : BONEWEIGHTS;
};

struct VSOutput {
    float4 position : SV_POSITION;
};

VSOutput main(VSInput input) {
    VSOutput output;
    const float4 local_pos = float4(input.position, 1.0f);
    const float4 bone_pos_x = mul(local_pos, bone_matrices[input.bones.x]) * input.weights.x;
    const float4 bone_pos_y = mul(local_pos, bone_matrices[input.bones.y]) * input.weights.y;
    const float4 skinned_pos = bone_pos_x + bone_pos_y;
    output.position = mul(skinned_pos, world);
    output.position = mul(output.position, light_view_proj);
    return output;
}
