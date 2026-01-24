#pragma once
#include "../../utility/Hash.h"

constexpr char MODEL_TEXTURE_NAME[] = "model_texture";
constexpr char SPHERE_TEXTURE_NAME[] = "sphere_texture";
constexpr char SHADOW_MAP_TEXTURE_NAME[] = "shadow_map";
constexpr char FXAA_TEXTURE_NAME[] = "fxaa_texture";
constexpr char ALPHA_MASK_INPUT_TEXTURE_NAME[] = "input_alpha_mask";
constexpr char ALPHA_MASK_OUTPUT_TEXTURE_NAME[] = "output_alpha_mask";

enum class TextureName : uint32_t {
    Model = hash_u32(MODEL_TEXTURE_NAME),
    Sphere = hash_u32(SPHERE_TEXTURE_NAME),
    ShadowMap = hash_u32(SHADOW_MAP_TEXTURE_NAME),
    FXAA = hash_u32(FXAA_TEXTURE_NAME),
    AlphaMaskInput = hash_u32(ALPHA_MASK_INPUT_TEXTURE_NAME),
    AlphaMaskOutput = hash_u32(ALPHA_MASK_OUTPUT_TEXTURE_NAME),
};
