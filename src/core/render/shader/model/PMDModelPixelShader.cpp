#include "PMDModelPixelShader.h"

std::filesystem::path PMDModelPixelShader::file_path(void) const {
    return L"assets/shader/ps_model.cso";
}

ShaderType PMDModelPixelShader::shader_type(void) const {
    return ShaderType::Pixel;
}

std::string PMDModelPixelShader::main_function_name(void) const {
    return "main";
}

std::string PMDModelPixelShader::version(void) const {
    return "ps_5_0";
}