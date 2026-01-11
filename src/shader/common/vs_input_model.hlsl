struct VSInput {
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    uint2 bones : BONEINDICES;
    float2 weights : BONEWEIGHTS;
};