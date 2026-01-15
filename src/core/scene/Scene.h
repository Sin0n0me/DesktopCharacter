#pragma once
#include "camera/SceneCamera.h"
#include "light/Light.h"

struct ID3D11DeviceContext;
struct ID3D11Device;

class Scene {
private:
    SceneCamera camera;
    Light light;

    std::shared_ptr<CommonResource> resource;

public:

    const SceneCamera& get_camera(void) const;
    const Light& get_lignt(void) const;

    Scene(ID3D11Device* const device, const std::shared_ptr<CommonResource>& resouce);

    bool init(ID3D11Device* const device);

    void update(const int64_t delta_time);

    void render_update(ID3D11DeviceContext* const context);

    void render(ID3D11DeviceContext* const context) const;
};
