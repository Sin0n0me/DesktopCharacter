#pragma once
#include "../../utility/Hash.h"

constexpr char MODEL_TEXTURE_NAME[] = "model_texture";
constexpr char SPHERE_TEXTURE_NAME[] = "sphere_texture";
constexpr char SHADOW_MAP_TEXTURE_NAME[] = "shadow_map";
constexpr char FXAA_TEXTURE_NAME[] = "fxaa_texture";

enum class TextureName : uint32_t {
	Model = hash_u32(MODEL_TEXTURE_NAME),
	Sphere = hash_u32(SPHERE_TEXTURE_NAME),
	ShadowMap = hash_u32(SHADOW_MAP_TEXTURE_NAME),
	FXAA = hash_u32(FXAA_TEXTURE_NAME),
};
