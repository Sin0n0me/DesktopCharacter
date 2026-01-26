#include "ModelEdgePixelShader.h"

std::filesystem::path ModelEdgePixelShader::file_path(void) const {
    return u8"assets/shader/ps_model_edge.cso";
}

ShaderType ModelEdgePixelShader::shader_type(void) const {
    return ShaderType::Pixel;
}

std::string ModelEdgePixelShader::main_function_name(void) const {
    return "main";
}

std::string ModelEdgePixelShader::version(void) const {
    return "ps_5_0";
}