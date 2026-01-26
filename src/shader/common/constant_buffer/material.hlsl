cbuffer Material {
    float4 diffuse;
    float3 specular;
    float shininess;
    float3 ambient;
    float sphere_mul; // ifによる分岐を減らすため ０-1 で計算
    float sphere_add;
};

float4 apply_lighting(
    const float4 base_color,
    const float3 position,
    const float3 light_direction,
    const float3 normal
){    
    const float3 normalized_normal = normalize(normal);
    const float3 normalized_light = normalize(light_direction);
    const float3 view_direction = normalize(-position);

    // Diffuse
    const float dot_nl = saturate(dot(normalized_normal, normalized_light));
    const float4 fixed_diffuse = diffuse * dot_nl;

    // Specular(Phong)
    /*
    const float3 ref = reflect(-normalized_light, normalized_normal);
    const float spec_factor = pow(saturate(dot(ref, view_direction)), shininess);
    const float3 fixed_specular = specular * spec_factor;
    */
  // Specular(Blinn-Phong)
    const float3 half_vec = normalize(normalized_light + view_direction);
    const float spec_factor = pow(saturate(dot(normalized_normal, half_vec)), shininess * 4);
    const float3 fixed_specular = specular * spec_factor;
    
    return float4(
        base_color.rgb * (ambient + fixed_diffuse.rgb) + fixed_specular,
        base_color.a
    );
}


float2 calc_sphere_uv(
    const float3 position,
    const float3 normal
) {
    const float3 view_dir = normalize(-position);
    const float3 reflect_dir = reflect(-view_dir, normalize(normal));
    const float dot_reflect = dot(reflect_dir.xy, reflect_dir.xy);
    const float pow_reflect = (reflect_dir.z + 1.0f) * (reflect_dir.z + 1.0f);
    const float m = 2.0f * sqrt(dot_reflect + pow_reflect);
    const float inv_m = rcp(max(m, 1e-5f));

    return float2(
        reflect_dir.x * inv_m + 0.5f,
        -reflect_dir.y * inv_m + 0.5f
    );
}

// IFによる分岐を減らすために以下のような回りくどい計算をしている
float4 apply_sphere_map_branchless(
    const float4 base_color,
    const float4 sphere_color
) {
    const float4 mul_case = (base_color * sphere_color) * sphere_mul;
    const float4 add_case = (base_color + sphere_color) * sphere_add;
    const float4 no_case = base_color * (1.0 - (sphere_mul + sphere_add)); // 適用なしの場合
    const float4 result = no_case + add_case + mul_case;
    return result;
}
