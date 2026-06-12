#include "d3d11_renderer.h"

namespace enishi::renderer::directx {
    void D3D11Renderer::bind(
        ID3D11DeviceContext* const context, const types::DrawCommand& command) const {
        switch (command.handle.type) {
            case types::RenderHandleType::Buffer: {
                const auto& opt_buffer = this->resource_manager.get_buffer(command.handle.id);
                if (!opt_buffer.has_value()) {
                    return;
                }
                const auto& buffer = opt_buffer.value();
                this->bind_buffer(context, buffer);
            } break;
            case types::RenderHandleType::Shader: {
                this->bind_shader(context, command.handle.id);
            } break;
            case types::RenderHandleType::Mesh: {
            } break;
            case types::RenderHandleType::Texture: {
            } break;
            case types::RenderHandleType::RenderTarget: {
            } break;
            default:
                break;
        }
    }

    void D3D11Renderer::bind_buffer(
        ID3D11DeviceContext* const context, const Buffer& buffer) const {
        switch (buffer.buffer_type) {
            case BufferType::Vertex: {
                const auto& vertex = buffer.parameter.vertex;
                context->IASetVertexBuffers(
                    0, 1, buffer.buffer.GetAddressOf(), &vertex.stride, &vertex.offset);
            } break;
            case BufferType::Index: {
                const auto& index = buffer.parameter.index;
                context->IASetIndexBuffer(buffer.buffer.Get(), index.format, index.offset);
            } break;
            case BufferType::Constant: {
                const auto& constant = buffer.parameter.constant;
                switch (constant.target_shader) {
                    case ShaderType::Vertex: {
                        context->VSSetConstantBuffers(
                            constant.target_slot, 1, buffer.buffer.GetAddressOf());
                    } break;
                    case ShaderType::Pixcel: {
                        context->PSSetConstantBuffers(
                            constant.target_slot, 1, buffer.buffer.GetAddressOf());
                    } break;
                    case ShaderType::Compute: {
                        context->CSSetConstantBuffers(
                            constant.target_slot, 1, buffer.buffer.GetAddressOf());
                    } break;
                    default:
                        break;
                }
            } break;
            default:
                break;
        }
    }

    void D3D11Renderer::bind_shader(
        ID3D11DeviceContext* const context, const types::HandleId id) const {
        const auto& pool = this->resource_manager.get_shader_pool();
        const auto opt_type = pool.get_shader_type(id);
        if (opt_type.is_none()) {
            return;
        }

        switch (opt_type.value()) {
            case ShaderType::Vertex: {
                const auto opt_shader = pool.get_vertex_shader(id);
                if (opt_shader.is_none()) {
                    return;
                }
                context->VSSetShader(opt_shader.value(), nullptr, 0);
            } break;
            case ShaderType::Pixcel: {
                const auto opt_shader = pool.get_pixel_shader(id);
                if (opt_shader.is_none()) {
                    return;
                }
                context->PSSetShader(opt_shader.value(), nullptr, 0);
            } break;
            case ShaderType::Compute: {
                const auto opt_shader = pool.get_compute_shader(id);
                if (opt_shader.is_none()) {
                    return;
                }
                context->CSSetShader(opt_shader.value(), nullptr, 0);
            } break;
            default:
                break;
        }
    }

    D3D11Renderer::D3D11Renderer(std::unique_ptr<D3D11> d3d11)
        : d3d11(std::move(d3d11))
        , resource_manager(ResourceManager{}) {
    }

    platform::RenderResult<types::RenderHandle> D3D11Renderer::create_mesh(
        const types::MeshData& mesh) {
        const auto device = this->d3d11->get_device();
        const auto result = this->resource_manager.make_mesh(device, mesh);
        if (result.is_err()) {
            return result.propagation(platform::RenderError::MakeError);
        }

        return result.value();
    }

    platform::RenderResult<types::RenderHandle> D3D11Renderer::create_texture(
        const types::TextureData& texture) {
        const auto device = this->d3d11->get_device();
        const auto result = this->resource_manager.make_texture(device, texture);
        if (result.is_err()) {
            return result.propagation(platform::RenderError::MakeError);
        }

        return result.value();
    }

    platform::RenderResult<types::RenderHandle> D3D11Renderer::create_shader(
        const types::ShaderData& shader) {
        const auto device = this->d3d11->get_device();
        const auto result = this->resource_manager.make_shader(device, shader);
        if (result.is_err()) {
            return result.propagation(platform::RenderError::MakeError);
        }

        return result.value();
    }

    void D3D11Renderer::submit_render_graph(const types::RenderGraph& graph) {
        graph.camera;

        const auto context = this->d3d11->get_context();
        for (const auto& pass : graph.passes) {
            pass.pass_type;

            pass.render_target;

            // context->OMSetRenderTargets(8, null_rtv, nullptr);

            // context->ClearRenderTargetView(render_target_view.Get(), CLEAR_COLOR);

            // context->RSSetState();

            for (const auto& command : pass.commands) {
                this->bind(context, command);
            }
        }
    }

    void D3D11Renderer::present(void) {
        this->d3d11->get_swap_chain()->Present(1, 0);
    }
} // namespace enishi::renderer::directx