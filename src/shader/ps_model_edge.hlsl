#include "common/input_ps_model_edge.hlsl"
#include "common/constant_buffer/model_edge.hlsl"

float4 main(const PSInput input) : SV_TARGET {
    return float4(edge_color, 1.0);
}
