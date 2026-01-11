#pragma once
#include "ShaderType.h"
#include <filesystem>
#include <string>

class IShaderBlueprint {
public:
	virtual ~IShaderBlueprint(void) = default;

	virtual std::filesystem::path file_path(void) const = 0;

	virtual ShaderType shader_type(void) const = 0;

	virtual std::string main_function_name(void) const = 0;

	virtual std::string version(void) const = 0;
};
