#include "../../Application.h"
#include "../render/CommonResource.h"
#include "Scene.h"
#include <d3d11.h>

const SceneCamera& Scene::get_camera(void) const {
    return this->camera;
}

const Light& Scene::get_lignt(void) const {
    return this->light;
}

Scene::Scene(ID3D11Device* const device, const std::shared_ptr<CommonResource>& resource) {
    this->resource = resource;
    this->camera = SceneCamera::make_camera(device, resource);
    this->light = Light::make_light(device, resource);

    //this->camera.camera.view = this->light.shadow.light_view_proj;
}

bool Scene::init(ID3D11Device* const device) {
    return true;
}

void Scene::update(const int64_t delta_time) {
}

static double degree = 0;
void Scene::render_update(ID3D11DeviceContext* const context) {
    /*
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
        &this->camera.camera,
        0,
        0
    );
    */
}

void Scene::render(ID3D11DeviceContext* const context) const {
}