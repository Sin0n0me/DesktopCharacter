#include "Scene.h"
#include <d3d11.h>
#include "../render_pass/CommonResource.h"

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

void Scene::update(void) {
}

void Scene::render_update(ID3D11DeviceContext* const context) {
	/*
	context->UpdateSubresource(
		this->resource->constant_buffers[ConstantBufferPattern::CameraBuffer].Get(),
		0,
		nullptr,
		&this->camera.camera,
		0,
		0
	);
	*/
}

void Scene::render(ID3D11DeviceContext* const context) const {
}