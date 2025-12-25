#include "Scene.h"

Scene::Scene(ID3D11Device* const device, const std::shared_ptr<CommonResource>& common_resouce) {
	this->camera = SceneCamera::make_camera(device, common_resouce);
	this->light = Light::make_light(device, common_resouce);
}

void Scene::update(void) {
}

void Scene::render(void) const {
}