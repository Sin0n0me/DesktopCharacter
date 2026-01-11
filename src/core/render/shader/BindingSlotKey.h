#pragma once
#include <cstdint>
#include "BindingSlotKind.h"
#include "ShaderType.h"
#include <xhash>

struct alignas(4) BindingSlotKey {
public:
	const uint32_t name_hash;
	const ShaderType shader_type;
	const BindingSlotKind slot_kind;

public:

	constexpr explicit BindingSlotKey(
		const uint32_t& name_hash,
		const ShaderType& shader_type,
		const BindingSlotKind& slot_kind
	) noexcept :
		name_hash(name_hash),
		shader_type(shader_type),
		slot_kind(slot_kind) {
	}

	bool operator==(const BindingSlotKey& rhs) const;
	bool operator!=(const BindingSlotKey& rhs) const;
};

namespace std {
	template <>
	struct hash<BindingSlotKey> {
		size_t operator()(const BindingSlotKey& p) const noexcept {
			std::size_t hash = p.name_hash;
			hash |= static_cast<std::size_t>(p.shader_type) << 32;
			hash |= static_cast<std::size_t>(p.slot_kind) << 48;
			return hash;
		}
	};
}
