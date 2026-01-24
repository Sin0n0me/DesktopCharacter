#pragma once
#include "../../utility/Hash.h"

constexpr char CAMERA_BUFFER_NAME[] = "Camera";
constexpr char SHADOW_MAP_BUFFER_NAME[] = "ShadowMap";
constexpr char BONES_BUFFER_NAME[] = "Bones";
constexpr char MATERIAL_BUFFER_NAME[] = "Material";
constexpr char FXAA_BUFFER_NAME[] = "FXAA";
constexpr char ALPHA_MASK_PARAMS_BUFFER_NAME[] = "AlphaMaskParams";

enum class ConstantBufferName : uint32_t {
    Camera = hash_u32(CAMERA_BUFFER_NAME),
    ShadowMap = hash_u32(SHADOW_MAP_BUFFER_NAME),
    Bones = hash_u32(BONES_BUFFER_NAME),
    Material = hash_u32(MATERIAL_BUFFER_NAME),
    FXAA = hash_u32(FXAA_BUFFER_NAME),
    AlphaMaskParams = hash_u32(ALPHA_MASK_PARAMS_BUFFER_NAME),
};
