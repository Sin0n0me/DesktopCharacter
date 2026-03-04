#pragma once
#include "InputElements.h"
#include <d3d11.h>
#include <d3d11shader.h>
#include <functional>
#include <optional>
#include <string>
#include <vector>

class ShaderRelfection {
private:
    static std::optional<D3D11_SHADER_INPUT_BIND_DESC> get_desc(
        void* const data,
        const uint32_t size,
        const std::function<bool(const D3D11_SHADER_INPUT_BIND_DESC&)>& function
    );

public:

    static void get_all_desc(
        void* const data,
        const uint32_t size,
        const std::function<void(const D3D11_SHADER_INPUT_BIND_DESC&)>& function
    );

    static std::optional<uint32_t> get_constant_buffer_slot(
        void* const data,
        const uint32_t size,
        const std::string& name
    );

    static std::optional<uint32_t> get_constant_buffer_slot(
        const std::vector<uint8_t>& data,
        const std::string& name
    );

    static std::optional<uint32_t> get_sampler_slot(
        void* const data,
        const uint32_t size,
        const std::string& name
    );

    static std::optional<uint32_t> get_sampler_slot(
        const std::vector<uint8_t>& data,
        const std::string& name
    );

    static std::optional<uint32_t> get_texture_slot(
        void* const data,
        const uint32_t size,
        const std::string& name
    );

    static std::optional<uint32_t> get_texture_slot(
        const std::vector<uint8_t>& data,
        const std::string& name
    );

    static std::optional<uint32_t> get_unordered_access_view_slot(
        void* const data,
        const uint32_t size,
        const std::string& name
    );

    static std::optional<uint32_t> get_unordered_access_view_slot(
        const std::vector<uint8_t>& data,
        const std::string& name
    );

    static std::optional<InputElements> get_input_elements(
        const std::vector<uint8_t>& data,
        const uint32_t& input_slot
    );
};
