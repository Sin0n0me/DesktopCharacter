#include "../../Application.h"
#include "../render/CommonResource.h"
#include "../render/constant_buffer/Camera.h"
#include "../render/constant_buffer/Shadow.h"
#include "Scene.h"
#include <d3d11.h>

std::shared_ptr<Camera> Scene::get_camera(void) const {
    return this->camera;
}

std::shared_ptr<Shadow> Scene::get_lignt(void) const {
    return this->light;
}

Scene::Scene(const std::shared_ptr<CommonResource>& resource) {
    // カメラ
    const float eye_position = 11.0f;
    const float distance = 25.0f;
    const Vector4 eye = DirectX::XMVectorSet(0, eye_position + 7.5f, -distance, 1.0f);
    const Vector4 target = DirectX::XMVectorSet(0.0f, eye_position, 0.0f, 1.0f);
    const Vector4 up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    this->camera = std::make_shared<Camera>(
        eye,
        target,
        up
    );

    // シャドウマップ用の光源
    const Vector4 light_position = DirectX::XMVectorSet(
        15.0f,
        30.0f,
        -30.0f,
        1.0f
    );
    const Vector4 light_target = DirectX::XMVectorSet(
        0.0f,
        10.0f,
        0.0f,
        0.0f
    );
    this->light = std::make_shared<Shadow>(
        light_position,
        light_target
    );

    this->resource = resource;
}

bool Scene::make_camera(ID3D11Device* const device) {
    constexpr D3D11_BUFFER_DESC bd{
        .ByteWidth = sizeof(Camera),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
    };
    const D3D11_SUBRESOURCE_DATA init_data{
        .pSysMem = this->camera.get(),
        .SysMemPitch = 0,
        .SysMemSlicePitch = sizeof(Camera),
    };

    const HRESULT hr = device->CreateBuffer(
        &bd,
        &init_data,
        resource->constant_buffers[ConstantBuffer::Camera].GetAddressOf()
    );
    if FAILED(hr) {
        return false;
    }

    return true;
}

bool Scene::make_shadow_map_light(ID3D11Device* const device) {
    constexpr D3D11_BUFFER_DESC desc{
        .ByteWidth = sizeof(Shadow),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
    };
    const D3D11_SUBRESOURCE_DATA init_data{
        .pSysMem = this->light.get(),
        .SysMemPitch = 0,
        .SysMemSlicePitch = sizeof(Shadow),
    };
    const HRESULT hr = device->CreateBuffer(
        &desc,
        &init_data,
        resource->constant_buffers[ConstantBuffer::ShadowMap].GetAddressOf()
    );
    if FAILED(hr) {
        return false;
    }

    return true;
}

bool Scene::init(ID3D11Device* const device) {
    if(!this->make_camera(device)) {
        return false;
    }

    if(!this->make_shadow_map_light(device)) {
        return false;
    }

    //this->camera.camera.view = this->light.shadow.light_view_proj;
    return true;
}

void Scene::update(const int64_t delta_time) {
}

void Scene::render_update(ID3D11DeviceContext* const context) {
    /*
    static double degree = 0;
    degree += 1;
    const double radius = 30.0;
    const double radian = degree * (3.141926535 / 180.0);
    const double x = radius * std::cos(radian);
    const double y = radius * std::sin(radian);

    const DirectX::XMVECTOR light_position = DirectX::XMVectorSet(
        x,
        30.0f,
        y,
        1.0f
    );
    const DirectX::XMVECTOR light_target = DirectX::XMVectorSet(
        0.0f,
        10.0f,
        0.0f,
        0.0f
    );
    this->light.shadow.light_view_proj = DirectX::XMMatrixTranspose(
        DirectX::XMMatrixLookAtLH(
            light_position,
            light_target,
            DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f)
        )
    );

    context->UpdateSubresource(
        this->resource->constant_buffers.at(ConstantBuffer::Camera).Get(),
        0,
        0,
        this->camera.get(),
        0,
        0
    );
    */
}

void Scene::render(ID3D11DeviceContext* const context) const {
}