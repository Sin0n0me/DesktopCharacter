#pragma once
#include <cstdint>

enum class BindingSlotKind : uint16_t {
	ConstantBuffer,
	SamplerState,
	Texture,
};