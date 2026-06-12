#include "shader.h"

namespace enishi::renderer::directx {
    foundation::Option<const ShaderInfo&> ShaderPool::get_shader_info(
        const types::HandleId id) const noexcept {
        const auto& iter = this->handle_map.find(id);
        if (iter == this->handle_map.end()) {
            return {};
        }

        return iter->second;
    }

    void ShaderPool::create(const types::HandleId id, const ShaderType shader_type) noexcept {
        switch (shader_type) {
            case ShaderType::Vertex: {
                this->vertex_shaders.push_back({});
                this->handle_map.emplace(id,
                    ShaderInfo{
                        .shader_type = shader_type,
                        .index = static_cast<std::uint32_t>(this->vertex_shaders.size()) - 1,
                    });
            } break;
            case ShaderType::Pixcel: {
                this->pixel_shaders.push_back({});
                this->handle_map.emplace(id,
                    ShaderInfo{
                        .shader_type = shader_type,
                        .index = static_cast<std::uint32_t>(this->pixel_shaders.size()) - 1,
                    });
            } break;
            case ShaderType::Compute: {
                this->compute_shaders.push_back({});
                this->handle_map.emplace(id,
                    ShaderInfo{
                        .shader_type = shader_type,
                        .index = static_cast<std::uint32_t>(this->compute_shaders.size()) - 1,
                    });
            } break;
            default:
                break;
        }
    }

    foundation::Option<ShaderType> ShaderPool::get_shader_type(
        const types::HandleId id) const noexcept {
        const auto opt_info = this->get_shader_info(id);
        if (!opt_info.has_value()) {
            return {};
        }
        const auto info = opt_info.unwrap();

        return info.shader_type;
    }

    foundation::Option<ID3D11VertexShader*> ShaderPool::get_vertex_shader(
        const types::HandleId id) const noexcept {
        const auto opt_info = this->get_shader_info(id);
        if (!opt_info.has_value()) {
            return {};
        }
        const auto info = opt_info.unwrap();

        if (info.shader_type != ShaderType::Vertex) {
            return {};
        }

        return this->vertex_shaders.at(info.index).Get();
    }

    foundation::Option<ID3D11PixelShader*> ShaderPool::get_pixel_shader(
        const types::HandleId id) const noexcept {
        const auto opt_info = this->get_shader_info(id);
        if (!opt_info.has_value()) {
            return {};
        }
        const auto info = opt_info.unwrap();

        if (info.shader_type != ShaderType::Pixcel) {
            return {};
        }
        return this->pixel_shaders.at(info.index).Get();
    }

    foundation::Option<ID3D11ComputeShader*> ShaderPool::get_compute_shader(
        const types::HandleId id) const noexcept {
        const auto opt_info = this->get_shader_info(id);
        if (!opt_info.has_value()) {
            return {};
        }
        const auto info = opt_info.unwrap();

        if (info.shader_type != ShaderType::Compute) {
            return {};
        }
        return this->compute_shaders.at(info.index).Get();
    }

    foundation::Option<ID3D11VertexShader* const*> ShaderPool::get_address_vertex_shader(
        const types::HandleId id) const noexcept {
        const auto opt_info = this->get_shader_info(id);
        if (!opt_info.has_value()) {
            return {};
        }
        const auto info = opt_info.unwrap();

        if (info.shader_type != ShaderType::Vertex) {
            return {};
        }
        return this->vertex_shaders.at(info.index).GetAddressOf();
    }

    foundation::Option<ID3D11PixelShader* const*> ShaderPool::get_address_pixel_shader(
        const types::HandleId id) const noexcept {
        const auto opt_info = this->get_shader_info(id);
        if (!opt_info.has_value()) {
            return {};
        }
        const auto info = opt_info.unwrap();

        if (info.shader_type != ShaderType::Pixcel) {
            return {};
        }
        return this->pixel_shaders.at(info.index).GetAddressOf();
    }

    foundation::Option<ID3D11ComputeShader* const*> ShaderPool::get_address_compute_shader(
        const types::HandleId id) const noexcept {
        const auto opt_info = this->get_shader_info(id);
        if (!opt_info.has_value()) {
            return {};
        }
        const auto info = opt_info.unwrap();

        if (info.shader_type != ShaderType::Compute) {
            return {};
        }
        return this->compute_shaders.at(info.index).GetAddressOf();
    }
} // namespace enishi::renderer::directx