#pragma once
#include "light/Light.h"

struct ID3D11DeviceContext;
struct ID3D11Device;

class Scene {
private:
    std::shared_ptr<Camera> camera;
    Light light;

    std::shared_ptr<CommonResource> resource;

    bool make_camera(ID3D11Device* const device);

public:

    const std::shared_ptr<Camera> get_camera(void) const;
    const Light& get_lignt(void) const;

    explicit Scene(const std::shared_ptr<CommonResource>& resouce);

    bool init(ID3D11Device* const device);

    void update(const int64_t delta_time);

    void render_update(ID3D11DeviceContext* const context);

    void render(ID3D11DeviceContext* const context) const;
};
