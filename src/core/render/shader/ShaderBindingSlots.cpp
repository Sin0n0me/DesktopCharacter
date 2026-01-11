#include "../../utility/Hash.h"
#include "ShaderBindingSlots.h"
#include "BindingSlotKey.h"
#include "Shader.h"

ShaderBindingSlots::ShaderBindingSlots(void) noexcept {
}

void ShaderBindingSlots::merge(const Shader& shader) {
	this->bind_map.merge(shader.get_all_slots());
}

bool ShaderBindingSlots::contains(
	const ShaderType& type,
	const BindingSlotKind& kind,
	const uint32_t& name_hash
) const noexcept {
	return this->contains(BindingSlotKey{name_hash, type, kind});
}

bool ShaderBindingSlots::contains(const BindingSlotKey& key) const noexcept {
	return this->bind_map.contains(key);
}

uint32_t ShaderBindingSlots::get(
	const ShaderType& type,
	const BindingSlotKind& kind,
	const uint32_t& name_hash
) const {
	return this->get(BindingSlotKey{name_hash, type, kind});
}

bool ShaderBindingSlots::get(const BindingSlotKey& key) const {
	return this->bind_map.at(key);
}

uint32_t ShaderBindingSlots::get_or(
	const ShaderType& type,
	const BindingSlotKind& kind,
	const uint32_t& name_hash,
	const uint32_t& or_value
) const noexcept {
	return this->get_or(BindingSlotKey{name_hash, type, kind}, or_value);
}

bool ShaderBindingSlots::get_or(
	const BindingSlotKey& key,
	const uint32_t& or_value
) const {
	const auto& iter = this->bind_map.find(key);
	if(iter == this->bind_map.end()) {
		return or_value;
	}
	return iter->second;
}