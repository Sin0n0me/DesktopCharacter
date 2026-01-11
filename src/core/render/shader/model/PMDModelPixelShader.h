#pragma once
#include "../IShaderBlueprint.h"
class PMDModelPixelShader : public IShaderBlueprint {
public:

	std::filesystem::path file_path(void) const override;
	ShaderType shader_type(void) const override;
	std::string main_function_name(void) const override;
	std::string version(void) const override;
};
