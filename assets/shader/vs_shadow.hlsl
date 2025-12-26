cbuffer Camera : register(b0) {
    matrix world;
    matrix view;
    matrix proj;
}

cbuffer Shadow : register(b1)
{
    matrix light_view_proj;
};

struct VSInput {
    float3 position : POSITION;
};

struct VSOutput {
    float4 position : SV_POSITION;
};

VSOutput main(VSInput input) {
    VSOutput output;
    output.position = mul(float4(input.position, 1.0f), world);
    output.position = mul(output.position, light_view_proj);
    return output;
}
