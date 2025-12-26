cbuffer Camera : register(b0) {
    matrix world;
    matrix view;
    matrix proj;
}

cbuffer Shadow : register(b2) {
    matrix light_view_proj;
};

Texture2D<float> shadow_map : register(t0);
SamplerComparisonState shadow_sampler : register(s0);
//SamplerState shadow_sampler : register(s0); 

struct VSInput {
    float3 position : POSITION;
};

struct PSInput {
    float4 position : SV_POSITION;
    float4 shadow : TEXCOORD0;
};

PSInput VSMain(VSInput input) {
    PSInput output;
    
    const float4 wpos = mul(float4(input.position, 1.0f), world);
    const float4 vpos = mul(wpos, view);
    output.position = mul(vpos, proj);
    
    output.shadow = mul(wpos, light_view_proj);
    output.shadow.xyz /= output.shadow.w;
    output.shadow.y *= -1.0f;
    output.shadow.xy = output.shadow.xy * 0.5f + 0.5f;
    
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET {       
    const float depth = input.shadow.z;
    const float shadow = shadow_map.SampleCmpLevelZero(
        shadow_sampler, 
        input.shadow.xy,
        depth
    );
 
    return float4(0.0f, 0.0f, 0.0f, 1.0f - shadow);

}
