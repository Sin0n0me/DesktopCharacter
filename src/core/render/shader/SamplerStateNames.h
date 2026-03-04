#pragma once
#include "../../utility/Hash.h"

constexpr char MODEL_SAMPER_STATE_NAME[] = "model_sampler";
constexpr char SPHERE_SAMPER_STATE_NAME[] = "sphere_sampler";
constexpr char SHADOW_MAP_SAMPER_STATE_NAME[] = "shadow_sampler";
constexpr char FXAA_SAMPER_STATE_NAME[] = "linear_sampler";
constexpr char TOON_SAMPER_STATE_NAME[] = "toon_sampler";

enum class SamplerStateName : uint32_t {
    Model = hash_u32(MODEL_SAMPER_STATE_NAME),
    Sphere = hash_u32(SPHERE_SAMPER_STATE_NAME),
    ShadowMap = hash_u32(SHADOW_MAP_SAMPER_STATE_NAME),
    FXAA = hash_u32(FXAA_SAMPER_STATE_NAME),
    Toon = hash_u32(TOON_SAMPER_STATE_NAME),
};
