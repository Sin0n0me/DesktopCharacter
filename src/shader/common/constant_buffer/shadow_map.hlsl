cbuffer ShadowMap {
    matrix light_view_proj;
};


float4 apply_shadow_map_light(float4 position) {    
    position = mul(position, light_view_proj);
    return position;
}
