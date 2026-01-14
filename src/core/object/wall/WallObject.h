#pragma once
#include "../IObjectRenderer.h"
#include <cstdint>
#include <wrl/client.h>

struct ID3D11Buffer;

class WallObject : public IObjectRenderer {
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> chair_vertex_buffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> chair_index_buffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> wall_vertex_buffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> wall_index_buffer;
    uint32_t chair_index_count;
    uint32_t wall_index_count;

public:
    explicit WallObject(void) noexcept;

    bool init(ID3D11Device* const device) override;
    void render_update(ID3D11DeviceContext* const context) override;
    void render(ID3D11DeviceContext* const context, const ShaderBindingSlots* slots) const override;

private:
    bool make_mesh_wall(ID3D11Device* const device);
    bool make_mesh_chair(ID3D11Device* const device);
    bool make_mesh(ID3D11Device* const device);
};
