#include "ShadowMapPixelShader.h"

std::filesystem::path ShadowMapPixelShader::file_path(void) const {
    return u8"assets/shader/ps_shadow_map.cso";
}

ShaderType ShadowMapPixelShader::shader_type(void) const {
    return ShaderType::Pixel;
}

std::string ShadowMapPixelShader::main_function_name(void) const {
    return "main";
}

std::string ShadowMapPixelShader::version(void) const {
    return "ps_5_0";
}