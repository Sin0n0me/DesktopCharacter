#include "BindingSlotKey.h"

bool BindingSlotKey::operator==(const BindingSlotKey& rhs) const {
	return this->name_hash == this->name_hash &&
		this->slot_kind == this->slot_kind &&
		this->shader_type == this->shader_type;
}

bool BindingSlotKey::operator!=(const BindingSlotKey& rhs) const {
	return this->name_hash != this->name_hash ||
		this->slot_kind != this->slot_kind ||
		this->shader_type != this->shader_type;
}