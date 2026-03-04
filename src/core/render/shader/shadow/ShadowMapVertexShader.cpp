#include "ShadowMapVertexShader.h"

std::filesystem::path ShadowMapVertexShader::file_path(void) const {
    return u8"assets/shader/vs_shadow_map.cso";
}

ShaderType ShadowMapVertexShader::shader_type(void) const {
    return ShaderType::Vertex;
}

std::string ShadowMapVertexShader::main_function_name(void) const {
    return "main";
}

std::string ShadowMapVertexShader::version(void) const {
    return "vs_5_0";
}