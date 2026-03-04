#include "ClearWallVertexShader.h"

std::filesystem::path ClearWallVertexShader::file_path(void) const {
    return u8"assets/shader/vs_clear_wall.cso";
}

ShaderType ClearWallVertexShader::shader_type(void) const {
    return ShaderType::Vertex;
}

std::string ClearWallVertexShader::main_function_name(void) const {
    return "main";
}

std::string ClearWallVertexShader::version(void) const {
    return "vs_5_0";
}