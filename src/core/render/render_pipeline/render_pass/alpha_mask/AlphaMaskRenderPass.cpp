#include "../../../../../Application.h"
#include "../../../../log/Logger.h"
#include "../../../CommonResource.h"
#include "../../../constant_buffer/AlphaMaskParams.h"
#include "../../../constant_buffer/ConstantBufferNames.h"
#include "../../../shader/alpha_mask/AlphaMaskComputeShader.h"
#include "../../../shader/Shader.h"
#include "../../../texrure/TextureNames.h"
#include "AlphaMaskRenderPass.h"
#include <d3d11.h>

constexpr std::uint32_t BUFFER_SIZE = WIDTH * HEIGHT;
constexpr float ALPHA_THRESHOLD = 0.25f;

AlphaMaskRenderPass::AlphaMaskRenderPass(const std::shared_ptr<CommonResource>& common_resource) noexcept : RenderPass(common_resource) {
    this->screen_alpha_buffers.resize(2);
    for(auto& buffer : this->screen_alpha_buffers) {
        buffer.resize(BUFFER_SIZE);
    }
    this->buffer_index = 0;
}

bool AlphaMaskRenderPass::init(
    ID3D11Device* const device,
    ID3D11RenderTargetView* const render_target_view
) {
    if(!this->make_shader_resouce_view(device, render_target_view)) {
        return false;
    }

    if(!this->make_buffers(device)) {
        return false;
    }

    if(!this->make_shader(device)) {
        return false;
    }

    return true;
}

void AlphaMaskRenderPass::update(ID3D11DeviceContext* const context) {
    this->buffer_index = this->get_next_buffer_index();
}

void AlphaMaskRenderPass::render_set(
    ID3D11DeviceContext* const context,
    ID3D11RenderTargetView* const render_target_view
) const {
    context->CSSetShaderResources(
        this->binding_slots->get(
            ShaderType::Compute,
            BindingSlotKind::Texture,
            static_cast<uint32_t>(TextureName::AlphaMaskInput)
        ),
        1,
        this->shader_resouce_view.GetAddressOf()
    );
    context->CSSetUnorderedAccessViews(
        this->binding_slots->get(
            ShaderType::Compute,
            BindingSlotKind::UnorderedAccessView,
            static_cast<uint32_t>(TextureName::AlphaMaskOutput)
        ),
        1,
        this->unordered_access_view.GetAddressOf(),
        nullptr
    );
    context->CSSetConstantBuffers(
        this->binding_slots->get(
            ShaderType::Compute,
            BindingSlotKind::ConstantBuffer,
            static_cast<uint32_t>(ConstantBufferName::AlphaMaskParams)
        ),
        1,
        this->resource->constant_buffers.at(ConstantBuffer::AlphaParams).GetAddressOf()
    );

    context->CSSetShader(
        this->resource->compute_shaders[Pattern::AlphaMask].Get(),
        nullptr,
        0
    );

    context->Dispatch(
        (WIDTH + 15) / 16,
        (HEIGHT + 15) / 16,
        1
    );

    ID3D11UnorderedAccessView* null_uav[] = {nullptr};
    ID3D11ShaderResourceView* null_srv[] = {nullptr};
    context->CSSetUnorderedAccessViews(0, 1, null_uav, nullptr);
    context->CSSetShaderResources(0, 1, null_srv);

    context->CopyResource(
        this->read_back_texture.Get(),
        this->output_texture.Get()
    );

    D3D11_MAPPED_SUBRESOURCE mapped{};
    context->Map(
        this->read_back_texture.Get(),
        0,
        D3D11_MAP_READ,
        0,
        &mapped
    );

    const auto index = this->get_next_buffer_index();
    std::memcpy(
        this->screen_alpha_buffers[index].data(),
        mapped.pData,
        this->screen_alpha_buffers[index].size()
    );

    context->Unmap(this->read_back_texture.Get(), 0);
}

void AlphaMaskRenderPass::render(ID3D11DeviceContext* const context) const {
}

bool AlphaMaskRenderPass::should_reset_state(void) const {
    return true;
}

bool AlphaMaskRenderPass::is_render_model(void) const {
    return false;
}

bool AlphaMaskRenderPass::is_post_render(void) const {
    return false;
}

void AlphaMaskRenderPass::back_buffer_resouce(
    ID3D11DeviceContext* const context,
    ID3D11ShaderResourceView* const shader_resouce_view
) const {
}

bool AlphaMaskRenderPass::make_buffers(ID3D11Device* const device) {
    if(!this->make_uav_buffer(device)) {
        return false;
    }

    if(!this->make_read_back_texture(device)) {
        return false;
    }

    if(!this->make_constant_buffer(device)) {
        return false;
    }

    return true;
}

bool AlphaMaskRenderPass::make_uav_buffer(ID3D11Device* const device) {
    {
        constexpr D3D11_TEXTURE2D_DESC desc{
            .Width = WIDTH,
            .Height = HEIGHT,
            .MipLevels = 1,
            .ArraySize = 1,
            .Format = DXGI_FORMAT_R8_UINT,
            .SampleDesc = {
                .Count = 1
        },
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_UNORDERED_ACCESS,
            .CPUAccessFlags = 0,
        };

        const HRESULT hr = device->CreateTexture2D(
            &desc,
            nullptr,
            this->output_texture.GetAddressOf()
        );
        if(FAILED(hr)) {
            Logger::error(u8"UAVテクスチャの作成に失敗しました");
            return false;
        }
    }

    {
        constexpr D3D11_UNORDERED_ACCESS_VIEW_DESC desc{
            .Format = DXGI_FORMAT_R8_UINT,
            .ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D,
        };

        const HRESULT hr = device->CreateUnorderedAccessView(
            this->output_texture.Get(),
            &desc,
            this->unordered_access_view.GetAddressOf()
        );
        if(FAILED(hr)) {
            Logger::error(u8"UAVの作成に失敗しました");
            return false;
        }
    }

    return true;
}

bool AlphaMaskRenderPass::make_read_back_texture(ID3D11Device* const device) {
    constexpr D3D11_TEXTURE2D_DESC desc{
        .Width = WIDTH,
        .Height = HEIGHT,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = DXGI_FORMAT_R8_UINT,
        .SampleDesc = {
            .Count = 1
    },
        .Usage = D3D11_USAGE_STAGING,
        .BindFlags = 0,
        .CPUAccessFlags = D3D11_CPU_ACCESS_READ,
    };

    const HRESULT hr = device->CreateTexture2D(
        &desc,
        nullptr,
        this->read_back_texture.ReleaseAndGetAddressOf()
    );
    if(FAILED(hr)) {
        Logger::error(u8"ReadBack用テクスチャの作成に失敗しました");
        return false;
    }

    return true;
}

bool AlphaMaskRenderPass::make_constant_buffer(ID3D11Device* const device) {
    constexpr AlphaMaskParams params{
        .width = WIDTH,
        .height = HEIGHT,
        .alpha_threshold = ALPHA_THRESHOLD,
    };
    constexpr D3D11_BUFFER_DESC desc{
        .ByteWidth = sizeof(decltype(params)),
        .Usage = D3D11_USAGE_IMMUTABLE,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
        .CPUAccessFlags = 0,
    };
    const D3D11_SUBRESOURCE_DATA init_data{
        .pSysMem = &params,
        .SysMemPitch = 0,
        .SysMemSlicePitch = sizeof(decltype(params)),
    };

    const HRESULT hr = device->CreateBuffer(
        &desc,
        &init_data,
        this->resource->constant_buffers[ConstantBuffer::AlphaParams].GetAddressOf()
    );
    if(FAILED(hr)) {
        Logger::error(u8"UVA用の定数バッファの作成に失敗しました");
        return false;
    }

    return true;
}

bool AlphaMaskRenderPass::make_shader(ID3D11Device* const device) {
    Shader compute_shader = Shader(std::make_unique<AlphaMaskComputeShader>());

    if(!compute_shader.make_shader(
        device,
        this->resource->compute_shaders[Pattern::AlphaMask].GetAddressOf()
    )) {
        return false;
    }

    this->binding_slots->merge(compute_shader);

    return true;
}

bool AlphaMaskRenderPass::make_shader_resouce_view(
    ID3D11Device* const device,
    ID3D11RenderTargetView* const render_target_view
) {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    D3D11_TEXTURE2D_DESC texture_desc{};
    {
        // RTVから元リソースを取得
        Microsoft::WRL::ComPtr<ID3D11Resource> resource;
        render_target_view->GetResource(&resource);

        // Texture2Dへ変換
        const HRESULT hr = resource.As(&texture);
        if(FAILED(hr)) {
            Logger::error(u8"SRV用のテクスチャ取得に失敗しました");
            return false;
        }

        // テクスチャ情報取得
        texture->GetDesc(&texture_desc);
    }

    {
        const D3D11_SHADER_RESOURCE_VIEW_DESC desc{
            .Format = texture_desc.Format,
            .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
            .Texture2D = D3D11_TEX2D_SRV{
                .MipLevels = texture_desc.MipLevels,
        },
        };
        const HRESULT hr = device->CreateShaderResourceView(
            texture.Get(),
            &desc,
            this->shader_resouce_view.GetAddressOf()
        );
        if(FAILED(hr)) {
            Logger::error(u8"SRVの作成に失敗しました");
            return false;
        }
    }

    return true;
}

uint64_t AlphaMaskRenderPass::get_next_buffer_index(void) const noexcept {
    const uint64_t next_index = this->buffer_index + 1;
    if(this->screen_alpha_buffers.size() < next_index + 1) {
        return 0;
    }
    return next_index;
}

RasterizerKind AlphaMaskRenderPass::rasterizer_kind(void) const {
    return RasterizerKind::CullBack;
}