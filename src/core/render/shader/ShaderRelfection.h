#pragma once
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

	static std::vector<D3D11_SHADER_INPUT_BIND_DESC> get_all_desc(
		void* const data,
		const uint32_t size
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

	static std::optional<std::vector<D3D11_INPUT_ELEMENT_DESC>> get_input_elements(
		const std::vector<uint8_t>& data,
		const uint32_t& input_slot
	);
};
