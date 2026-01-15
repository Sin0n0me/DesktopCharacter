#include "FXAAPixelShader.h"

std::filesystem::path FXAAPixelShader::file_path(void) const {
    return L"assets/shader/ps_fxaa.cso";
}

ShaderType FXAAPixelShader::shader_type(void) const {
    return ShaderType::Pixel;
}

std::string FXAAPixelShader::main_function_name(void) const {
    return "main";
}

std::string FXAAPixelShader::version(void) const {
    return "ps_5_0";
}