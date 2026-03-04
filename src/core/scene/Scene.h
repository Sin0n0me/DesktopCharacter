#pragma once
#include <memory>

struct ID3D11DeviceContext;
struct ID3D11Device;
struct Camera;
struct Shadow;
class CommonResource;

class Scene {
private:
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Shadow> light;

    std::shared_ptr<CommonResource> resource;

    bool make_camera(ID3D11Device* const device);
    bool make_shadow_map_light(ID3D11Device* const device);

public:

    std::shared_ptr<Camera> get_camera(void) const;
    std::shared_ptr<Shadow> get_lignt(void) const;

    explicit Scene(const std::shared_ptr<CommonResource>& resouce);

    bool init(ID3D11Device* const device);

    void update(const int64_t delta_time);

    void render_update(ID3D11DeviceContext* const context);

    void render(ID3D11DeviceContext* const context) const;
};
