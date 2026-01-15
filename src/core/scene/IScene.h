#pragma once
#include <cstdint>

struct ID3D11DeviceContext;
struct ID3D11Device;

class IScene {
public:

    virtual ~IScene(void) = default;

    virtual bool init(void) = 0;

    virtual void update(const int64_t delta_time) = 0;

    virtual bool render_init(ID3D11Device* const device) = 0;

    virtual void render_update(ID3D11DeviceContext* const context) = 0;

    virtual void render(ID3D11DeviceContext* const context) const = 0;
};
