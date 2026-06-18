#include "d3d11_renderer.h"

namespace enishi::renderer::directx {
    constexpr float CLEAR_COLOR[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    void D3D11Renderer::execute(
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
            case types::RenderHandleType::View: {
                this->bind_render_target(context, command.handle.id);
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

    void D3D11Renderer::bind_render_target(
        ID3D11DeviceContext* const context, const types::HandleId id) const {
        const auto& view_pool = this->resource_manager.get_view_pool();
        const auto opt_type = view_pool.get_view_type(id);
        if (opt_type.is_none()) {
            return;
        }

        switch (opt_type.value()) {
            case types::ImageViewType::DepthStencil: {
            } break;
            case types::ImageViewType::RenderTarget: {
                const auto opt_view = view_pool.get_address_render_target_view(id);
                if (opt_view.is_none()) {
                    return;
                }
                context->OMSetRenderTargets(1, opt_view.value(), nullptr);
            } break;
            case types::ImageViewType::ShaderResource: {
            } break;
            case types::ImageViewType::UnorderedAccess: {
            } break;
            default:
                break;
        }
    }

    D3D11Renderer::D3D11Renderer(std::unique_ptr<D3D11> d3d11)
        : d3d11(std::move(d3d11))
        , resource_manager(ResourceManager{}) {
    }

    platform::RenderResult<types::RenderHandle> D3D11Renderer::create_viewport(
        const types::ViewportRect& config) {
        return platform::RenderResult<types::RenderHandle>();
    }

    platform::RenderResult<std::unique_ptr<platform::IPipelineLayout>>
    D3D11Renderer::create_pipeline_layout(const types::VertexLayout& layout,
        const types::RenderHandle& vertex_shader,
        const types::RenderHandle& pixel_shader) {
        return platform::RenderResult<std::unique_ptr<platform::IPipelineLayout>>();
    }

    platform::RenderResult<types::RenderHandle> D3D11Renderer::create_rasterizer(void) {
        return platform::RenderResult<types::RenderHandle>();
    }

    platform::RenderResult<types::RenderHandle> D3D11Renderer::create_image(
        const types::ImageDescription& description) {
        return platform::RenderResult<types::RenderHandle>();
    }

    platform::RenderResult<std::unique_ptr<platform::IRenderTargetView>>
    D3D11Renderer::create_render_target_view(
        types::RenderHandle image_handle, const types::ImageViewDescription& description) {
        return platform::RenderResult<std::unique_ptr<platform::IRenderTargetView>>();
    }

    platform::RenderResult<std::unique_ptr<platform::IDepthStencilView>>
    D3D11Renderer::create_depth_stencil_view(
        types::RenderHandle image_handle, const types::ImageViewDescription& description) {
        return platform::RenderResult<std::unique_ptr<platform::IDepthStencilView>>();
    }

    platform::RenderResult<std::unique_ptr<platform::IShaderResourceView>>
    D3D11Renderer::create_shader_resource_view(
        types::RenderHandle image_handle, const types::ImageViewDescription& description) {
        return platform::RenderResult<std::unique_ptr<platform::IShaderResourceView,
            std::default_delete<platform::IShaderResourceView>>>();
    }

    platform::RenderResult<std::unique_ptr<platform::IUnorderedAccessView,
        std::default_delete<platform::IUnorderedAccessView>>>
    D3D11Renderer::create_unordered_access_view(
        types::RenderHandle image_handle, const types::ImageViewDescription& description) {
        return platform::RenderResult<std::unique_ptr<platform::IUnorderedAccessView,
            std::default_delete<platform::IUnorderedAccessView>>>();
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
        const auto& context = this->d3d11->get_context();
        const auto& view_pool = this->resource_manager.get_view_pool();

        for (const auto& render_target : view_pool.get_render_targets()) {
            context->ClearRenderTargetView(render_target.Get(), CLEAR_COLOR);
        }

        for (const auto& pass : graph.passes) {
            switch (pass.pass_type) {
                case types::RenderPassType::Lighting: {
                } break;
                case types::RenderPassType::Model: {
                } break;
                case types::RenderPassType::PostProcess: {
                } break;
                case types::RenderPassType::Shadow: {
                } break;
                case types::RenderPassType::Transparent: {
                } break;
                case types::RenderPassType::UI: {
                } break;
                default:
                    break;
            }

            pass.render_target;

            //

            // context->RSSetState();

            for (const auto& command : pass.commands) {
                this->execute(context, command);
            }
        }
    }

    void D3D11Renderer::present(void) {
        this->d3d11->get_swap_chain()->Present(1, 0);
    }
} // namespace enishi::renderer::directx