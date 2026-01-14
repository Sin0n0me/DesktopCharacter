#include "../../render/CommonResource.h"
#include "Light.h"
#include <d3d11.h>

Light Light::make_light(ID3D11Device* const device, const std::shared_ptr<CommonResource>& resource) {
    // 光源
    const DirectX::XMVECTOR light_position = DirectX::XMVectorSet(
        20.0f,
        30.0f,
        -30.0f,
        1.0f
    );
    const DirectX::XMVECTOR light_target = DirectX::XMVectorSet(
        0.0f,
        10.0f,
        0.0f,
        0.0f
    );
    const auto view = DirectX::XMMatrixLookAtLH(
        light_position,
        light_target,
        DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
    );
    const auto proj = DirectX::XMMatrixOrthographicLH(
        100.0f,
        100.0f,
        0.1f,
        100.0f
    );
    const Shadow shadow{
        .light_view_proj = DirectX::XMMatrixTranspose(
            view * proj
        ),
    };
    const Light light{
        .shadow = shadow
    };

    constexpr D3D11_BUFFER_DESC desc{
        .ByteWidth = sizeof(Shadow),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
    };
    const D3D11_SUBRESOURCE_DATA init_data{
        .pSysMem = &shadow,
        .SysMemPitch = 0,
        .SysMemSlicePitch = sizeof(Shadow),
    };
    const HRESULT hr = device->CreateBuffer(
        &desc,
        &init_data,
        resource->constant_buffers[ConstantBuffer::ShadowMap].GetAddressOf()
    );
    if FAILED(hr) {
        throw;
    }

    return light;
}