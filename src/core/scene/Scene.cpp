#include "Scene.h"
#include <d3d11.h>
#include "../render/CommonResource.h"

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

void Scene::render_update(ID3D11DeviceContext* const context) {
}

void Scene::render(ID3D11DeviceContext* const context) const {
}