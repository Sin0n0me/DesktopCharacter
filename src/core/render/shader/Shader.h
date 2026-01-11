#pragma once

#include "BindingSlotKind.h"
#include "IShaderBlueprint.h"
#include <d3dcommon.h>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "BindingSlotKey.h"

namespace std::filesystem {
	class path;
};

struct ID3D11Device;
struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

class Shader {
protected:
	std::unique_ptr<IShaderBlueprint> blueprint;
	std::vector<uint8_t> shader_raw_data;

private:

	bool compile_vertex_shader(
		ID3D11Device* const device,
		ID3D11VertexShader** const vertex_shader
	);

	bool compile_pixel_shader(
		ID3D11Device* const device,
		ID3D11PixelShader** const pixel_shader
	);

	bool make_vertex_shader(
		ID3D11Device* const device,
		ID3D11VertexShader** const vertex_shader
	);

	bool make_pixel_shader(
		ID3D11Device* const device,
		ID3D11PixelShader** const pixel_shader
	);

	bool compile_shader(ID3DBlob** const lob);

	static bool is_compiled_shader_file(const std::filesystem::path& path);

public:

	explicit Shader(std::unique_ptr<IShaderBlueprint> blueprint);

	bool read_shader(void);

	template<typename T>
	bool make_shader(
		ID3D11Device* const device,
		T** const shader
	);

	template<>
	bool make_shader<ID3D11VertexShader>(
		ID3D11Device* const device,
		ID3D11VertexShader** const shader
	);

	template<>
	bool make_shader<ID3D11PixelShader>(
		ID3D11Device* const device,
		ID3D11PixelShader** const shader
	);

	bool make_input_layout(
		ID3D11Device* const device,
		ID3D11InputLayout** const input_layout
	);

	std::optional<uint32_t> get_constant_buffer_slot(const std::string& name) const;

	bool read_constant_buffer_slot(
		uint32_t& slot,
		const std::string& name
	) const;

	std::optional<uint32_t> get_sampler_state_slot(const std::string& name) const;

	std::optional<uint32_t> get_texture_slot(const std::string& name) const;

	bool read_sampler_state_slot(
		uint32_t& slot,
		const std::string& name
	) const;

	const IShaderBlueprint* get_blueprint(void) const;

	std::unordered_map<BindingSlotKey, uint32_t> get_all_slots(void) const;
};

template<typename T>
inline bool Shader::make_shader(ID3D11Device* const device, T** const shader) {
	return false;
}

template<>
inline bool Shader::make_shader(ID3D11Device* const device, ID3D11VertexShader** const shader) {
	return this->make_vertex_shader(device, shader);
}

template<>
inline bool Shader::make_shader(ID3D11Device* const device, ID3D11PixelShader** const shader) {
	return this->make_pixel_shader(device, shader);
}
