#include "BindingSlotKey.h"

bool BindingSlotKey::operator==(const BindingSlotKey& rhs) const {
    return this->name_hash == rhs.name_hash
        && this->slot_kind == rhs.slot_kind
        && this->shader_type == rhs.shader_type;
}

bool BindingSlotKey::operator!=(const BindingSlotKey& rhs) const {
    return this->name_hash != rhs.name_hash
        || this->slot_kind != rhs.slot_kind
        || this->shader_type != rhs.shader_type;
}