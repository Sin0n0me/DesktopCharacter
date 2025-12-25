cbuffer Shadow : register(b0)
{
    matrix world;
    matrix light_view_proj;
};

struct VSInput {
    float3 pos : POSITION;
};

struct VSOutput {
    float4 pos : SV_POSITION;
};

VSOutput main(VSInput input) {
    float4 p = float4(input.pos, 1);

    VSOutput output;
    output.pos = mul(input.pos, light_view_proj);
    return output;
}
