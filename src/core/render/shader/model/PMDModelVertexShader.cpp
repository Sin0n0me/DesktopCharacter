#include "PMDModelVertexShader.h"

std::filesystem::path PMDModelVertexShader::file_path(void) const {
    return u8"assets/shader/vs_model.cso";
}

ShaderType PMDModelVertexShader::shader_type(void) const {
    return ShaderType::Vertex;
}

std::string PMDModelVertexShader::main_function_name(void) const {
    return "main";
}

std::string PMDModelVertexShader::version(void) const {
    return "vs_5_0";
}