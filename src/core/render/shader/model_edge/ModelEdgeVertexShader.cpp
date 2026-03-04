#include "ModelEdgeVertexShader.h"

std::filesystem::path ModelEdgeVertexShader::file_path(void) const {
    return u8"assets/shader/vs_model_edge.cso";
}

ShaderType ModelEdgeVertexShader::shader_type(void) const {
    return ShaderType::Vertex;
}

std::string ModelEdgeVertexShader::main_function_name(void) const {
    return "main";
}

std::string ModelEdgeVertexShader::version(void) const {
    return "vs_5_0";
}