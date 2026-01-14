#include "ClearWallPixelShader.h"

std::filesystem::path ClearWallPixelShader::file_path(void) const {
	return L"assets/shader/ps_clear_wall.cso";
}

ShaderType ClearWallPixelShader::shader_type(void) const {
	return ShaderType::Pixel;
}

std::string ClearWallPixelShader::main_function_name(void) const {
	return "main";
}

std::string ClearWallPixelShader::version(void) const {
	return "ps_5_0";
}