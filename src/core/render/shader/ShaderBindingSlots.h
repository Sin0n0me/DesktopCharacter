#pragma once

#include "BindingSlotKey.h"
#include "ShaderType.h"
#include <unordered_map>

class Shader;

class ShaderBindingSlots {
private:
    std::unordered_map<BindingSlotKey, uint32_t> bind_map;

public:

    explicit ShaderBindingSlots(void) noexcept;

    void merge(const Shader& shader);

    bool contains(
        const ShaderType& type,
        const BindingSlotKind& kind,
        const uint32_t& name_hash
    ) const noexcept;

    bool contains(const BindingSlotKey& key) const noexcept;

    uint32_t get(
        const ShaderType& type,
        const BindingSlotKind& kind,
        const uint32_t& name_hash
    ) const;

    uint32_t get(const BindingSlotKey& key) const;

    uint32_t get_or(
        const ShaderType& type,
        const BindingSlotKind& kind,
        const uint32_t& name_hash,
        const uint32_t& or_value
    ) const noexcept;

    uint32_t get_or(
        const BindingSlotKey& key,
        const uint32_t& or_value
    ) const;
};
