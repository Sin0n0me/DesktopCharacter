#include "FXAAVertexShader.h"

std::filesystem::path FXAAVertexShader::file_path(void) const {
    return u8"assets/shader/vs_fxaa.cso";
}

ShaderType FXAAVertexShader::shader_type(void) const {
    return ShaderType::Vertex;
}

std::string FXAAVertexShader::main_function_name(void) const {
    return "main";
}

std::string FXAAVertexShader::version(void) const {
    return "vs_5_0";
}