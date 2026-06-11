#include "resource_manager.h"

namespace enishi::renderer::directx {
    ResourceManager::Result ResourceManager::make_mesh(
        ID3D11Device* const device, const types::MeshData& mesh) {
        auto result_vertex = this->make_vertex_buffer(device, mesh.vertices);
        if (result_vertex.is_err()) {
            return result_vertex.add_message(u8"メッシュの作成に失敗しました").error();
        }
        const auto vertex_handle = result_vertex.value();

        const auto result_index = this->make_index_buffer(device, mesh.indices);
        if (result_index.is_err()) {
            return result_vertex.add_message(u8"メッシュの作成に失敗しました").error();
        }
        const auto index_handle = result_index.value();

        const types::HandleId handle = this->handle_allocator.create();
        const Mesh mesh{
            .vertex_handle = vertex_handle,
            .index_handle = index_handle,
        };
        this->meshes.emplace(handle, mesh);

        return types::RenderHandle{
            .id = handle,
            .type = types::RenderHandleType::Mesh,
        };
    }

    ResourceManager::Result ResourceManager::make_vertex_buffer(
        ID3D11Device* const device, const types::RenderData& data) {
        const D3D11_BUFFER_DESC desc{
            .ByteWidth = data.byte_width,
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
        };
        const D3D11_SUBRESOURCE_DATA init_data{
            .pSysMem = data.data,
        };

        const BufferParameter parameter = BufferParameter{
            .vertex =
                VertexParameter{
                    .stride = data.stride,
                    .offset = 0,
                },
        };
        Buffer buffer{
            .buffer_type = BufferType::Vertex,
            .parameter = parameter,
        };
        const HRESULT hr = device->CreateBuffer(&desc, &init_data, buffer.buffer.GetAddressOf());
        if FAILED (hr) {
            return foundation::Error(
                DirectXError::BufferError, u8"頂点バッファの作成に失敗しました");
        }

        const types::HandleId handle = this->handle_allocator.create();
        this->resource.buffers.emplace(handle, buffer);

        return types::RenderHandle{
            .id = handle,
            .type = types::RenderHandleType::Buffer,
        };
    }

    ResourceManager::Result ResourceManager::make_index_buffer(
        ID3D11Device* const device, const types::RenderData& data) {
        const D3D11_BUFFER_DESC desc{
            .ByteWidth = data.byte_width,
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_INDEX_BUFFER,
            .CPUAccessFlags = 0,
        };
        const D3D11_SUBRESOURCE_DATA init_data{
            .pSysMem = data.data,
        };

        const DXGI_FORMAT format = [](const std::uint32_t stride) -> DXGI_FORMAT {
            switch (stride) {
                case 1:
                    return DXGI_FORMAT::DXGI_FORMAT_R8_UINT;
                case 2:
                    return DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
                case 4:
                    return DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
                default:
                    break;
            }
            return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
        }(data.stride);

        const BufferParameter parameter = BufferParameter{
            .index = IndexParameter{.format = format},
        };
        Buffer buffer{
            .buffer_type = BufferType::Index,
            .parameter = parameter,
        };
        const HRESULT hr = device->CreateBuffer(&desc, &init_data, buffer.buffer.GetAddressOf());
        if FAILED (hr) {
            return foundation::Error(
                DirectXError::BufferError, u8"インデックスバッファの作成に失敗しました");
        }

        const types::HandleId handle = this->handle_allocator.create();
        this->resource.buffers.emplace(handle, buffer);

        return types::RenderHandle{
            .id = handle,
            .type = types::RenderHandleType::Buffer,
        };
    }

    ResourceManager::Result ResourceManager::make_constant_buffer(
        ID3D11Device* const device, const types::RenderData& data) {
        const D3D11_BUFFER_DESC desc{
            .ByteWidth = data.byte_width,
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = 0,
        };
        const D3D11_SUBRESOURCE_DATA init_data{
            .pSysMem = data.data,
            .SysMemPitch = 0,
            .SysMemSlicePitch = data.stride,
        };

        Buffer buffer{
            .buffer_type = BufferType::Constant,
        };
        const HRESULT hr = device->CreateBuffer(&desc, &init_data, buffer.buffer.GetAddressOf());
        if FAILED (hr) {
            return foundation::Error(
                DirectXError::BufferError, u8"定数バッファの作成に失敗しました");
        }

        const types::HandleId handle = this->handle_allocator.create();
        this->resource.buffers.emplace(handle, buffer);

        return types::RenderHandle{
            .id = handle,
            .type = types::RenderHandleType::Buffer,
        };
    }

    ResourceManager::Result ResourceManager::make_texture(ID3D11Device* const device,
        const types::RenderData& data,
        const std::uint32_t width,
        const std::uint32_t height) {
        const D3D11_SUBRESOURCE_DATA subresource{
            .pSysMem = data.data,
            .SysMemPitch = width * 4,
        };
        constexpr DXGI_SAMPLE_DESC sample{.Count = 1};
        const D3D11_TEXTURE2D_DESC desc{
            .Width = width,
            .Height = height,
            .MipLevels = 1,
            .ArraySize = 1,
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .SampleDesc = sample,
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_SHADER_RESOURCE,
        };

        Texture texture{
            .texture_type = TextureType::Texture2D,
        };
        const HRESULT hr =
            device->CreateTexture2D(&desc, &subresource, texture.texture.GetAddressOf());
        if (FAILED(hr)) {
            return foundation::Error(DirectXError::BufferError, u8"テクスチャの作成に失敗しました");
        }

        const types::HandleId handle = this->handle_allocator.create();
        this->resource.textures.emplace(handle, texture);

        return types::RenderHandle{
            .id = handle,
            .type = types::RenderHandleType::Texture,
        };
    }

    std::optional<const Buffer&> ResourceManager::get_buffer(const types::HandleId handle) const {
        const auto& iter = this->resource.buffers.find(handle);
        if (iter == this->resource.buffers.end()) {
            return {};
        }
        return iter->second;
    }

    const ShaderPool& ResourceManager::get_shader_pool(void) const {
        return this->resource.shaders;
    }
} // namespace enishi::renderer::directx