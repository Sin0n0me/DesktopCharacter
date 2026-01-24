#include "AlphaMaskComputeShader.h"

std::filesystem::path AlphaMaskComputeShader::file_path(void) const {
    return u8"assets/shader/cs_alpha_mask.cso";
}

ShaderType AlphaMaskComputeShader::shader_type(void) const {
    return ShaderType::Compute;
}

std::string AlphaMaskComputeShader::main_function_name(void) const {
    return "main";
}

std::string AlphaMaskComputeShader::version(void) const {
    return "cs_5_0";
}