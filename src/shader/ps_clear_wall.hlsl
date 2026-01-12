#include "common/ps_input_clear_wall.hlsl"

Texture2D<float> shadow_map;
SamplerComparisonState shadow_sampler;
//SamplerState shadow_sampler;

float4 main(const PSInput input) : SV_TARGET {
    /*
    float3 shadowCoord = input.position.xyz / input.position.w;
    shadowCoord.x = shadowCoord.x * 0.5f + 0.5f;
    shadowCoord.y = -shadowCoord.y * 0.5f + 0.5f;

    // 比較サンプラーではなく、通常のサンプラーで深度値を読み取る
    // (t0にセットしたshadowSRVをそのまま読み取る)
    float depth = shadow_map.Sample(shadow_sampler, shadowCoord.xy).r;

    // 深度値を色として出力
    return float4(depth, depth, depth, 1.0f);
    */

    const float depth = input.shadow.z;
    const float shadow = shadow_map.SampleCmpLevelZero(
        shadow_sampler, 
        input.shadow.xy,
        depth
    );
    
    // ある程度影は透過させる
    const float alpha = (1.0f - shadow) * 0.25;
 
    return float4(0.0f, 0.0f, 0.0f, alpha);
}
