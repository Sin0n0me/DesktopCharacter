#include "WallObject.h"
#include <d3d11.h>
#include <DirectXMath.h>

constexpr float WALL_SIZE = 30.0f;
constexpr float HALF_WALL_SIZE = WALL_SIZE / 2.0f;
constexpr float FRONT_DEPTH = 0.0f;
constexpr float FLOOR_DEPTH = 1.5f; // 奥行(床面)
constexpr float OFFSET_Y = 0.0f;

struct ShadowReceiverVertex {
    DirectX::XMFLOAT3 position;
};

WallObject::WallObject(void) noexcept {
    this->chair_index_count = 0;
    this->wall_index_count = 0;
    this->is_sitting = false;
}

bool WallObject::init(ID3D11Device* const device) {
    if(!this->make_mesh(device)) {
        return false;
    }

    return true;
}

void WallObject::render_update(ID3D11DeviceContext* const context) {
}

void WallObject::render(
    ID3D11DeviceContext* const context,
    const ShaderBindingSlots* slots
) const {
    const UINT stride = sizeof(ShadowReceiverVertex);
    const UINT offset = 0;

    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );

    if(this->is_sitting) {
        context->IASetVertexBuffers(
            0,
            1,
            this->chair_vertex_buffer.GetAddressOf(),
            &stride,
            &offset
        );
        context->IASetIndexBuffer(
            this->chair_index_buffer.Get(),
            DXGI_FORMAT_R32_UINT,
            0
        );
        context->DrawIndexed(this->chair_index_count, 0, 0);
    } else {
        context->IASetVertexBuffers(
            0,
            1,
            this->wall_vertex_buffer.GetAddressOf(),
            &stride,
            &offset
        );
        context->IASetIndexBuffer(
            this->wall_index_buffer.Get(),
            DXGI_FORMAT_R32_UINT,
            0
        );
        context->DrawIndexed(this->wall_index_count, 0, 0);
    }
}

bool WallObject::make_mesh_wall(ID3D11Device* const device) {
    constexpr ShadowReceiverVertex WALL_VERTICES[] = {
        {{-HALF_WALL_SIZE, -HALF_WALL_SIZE, FRONT_DEPTH + FLOOR_DEPTH}},
        {{-HALF_WALL_SIZE, WALL_SIZE, FRONT_DEPTH + FLOOR_DEPTH}},
        {{HALF_WALL_SIZE, WALL_SIZE, FRONT_DEPTH + FLOOR_DEPTH}},
        {{HALF_WALL_SIZE, -HALF_WALL_SIZE, FRONT_DEPTH + FLOOR_DEPTH}},
    };
    constexpr uint32_t INDICES[] = {
        0, 1, 2,
        0, 2, 3,
    };

    this->wall_index_count = sizeof(INDICES) / sizeof(uint32_t);

    {
        D3D11_BUFFER_DESC desc{};
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.ByteWidth = sizeof(WALL_VERTICES);
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA init_data{};
        init_data.pSysMem = WALL_VERTICES;

        const HRESULT hr = device->CreateBuffer(
            &desc,
            &init_data,
            this->wall_vertex_buffer.GetAddressOf()
        );
        if(FAILED(hr)) {
            return false;
        }
    }

    {
        D3D11_BUFFER_DESC desc{};
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.ByteWidth = sizeof(INDICES);
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA init_data{};
        init_data.pSysMem = INDICES;

        const HRESULT hr = device->CreateBuffer(
            &desc,
            &init_data,
            this->wall_index_buffer.GetAddressOf()
        );
        if(FAILED(hr)) {
            return false;
        }
    }

    return true;
}

bool WallObject::make_mesh_chair(ID3D11Device* const device) {
    constexpr ShadowReceiverVertex CHAIR_VERTICES[] = {
        // Wall
        {{-HALF_WALL_SIZE, OFFSET_Y, FRONT_DEPTH + FLOOR_DEPTH}},
        {{-HALF_WALL_SIZE, WALL_SIZE + OFFSET_Y, FRONT_DEPTH + FLOOR_DEPTH}},
        {{HALF_WALL_SIZE, WALL_SIZE + OFFSET_Y, FRONT_DEPTH + FLOOR_DEPTH}},
        {{HALF_WALL_SIZE, OFFSET_Y, FRONT_DEPTH + FLOOR_DEPTH}},

        // Floor
        {{-HALF_WALL_SIZE, OFFSET_Y, FRONT_DEPTH}},
        {{-HALF_WALL_SIZE, OFFSET_Y, FRONT_DEPTH + FLOOR_DEPTH}},
        {{HALF_WALL_SIZE, OFFSET_Y, FRONT_DEPTH + FLOOR_DEPTH}},
        {{HALF_WALL_SIZE, OFFSET_Y, FRONT_DEPTH}},

        // UnderWall
        {{-HALF_WALL_SIZE, -HALF_WALL_SIZE + OFFSET_Y, FRONT_DEPTH}},
        {{-HALF_WALL_SIZE, OFFSET_Y, FRONT_DEPTH}},
        {{HALF_WALL_SIZE, OFFSET_Y, FRONT_DEPTH}},
        {{HALF_WALL_SIZE, -HALF_WALL_SIZE + OFFSET_Y, FRONT_DEPTH}},
    };

    constexpr uint32_t INDICES[] = {
        0, 1, 2,
        0, 2, 3,
        4, 5, 7,
        5, 6, 7,
        10, 8, 9,
        10, 11, 8,
    };

    this->chair_index_count = sizeof(INDICES) / sizeof(uint32_t);

    {
        D3D11_BUFFER_DESC desc{};
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.ByteWidth = sizeof(CHAIR_VERTICES);
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vb_data{};
        vb_data.pSysMem = CHAIR_VERTICES;

        const HRESULT hr = device->CreateBuffer(
            &desc,
            &vb_data,
            this->chair_vertex_buffer.GetAddressOf()
        );
        if(FAILED(hr)) {
            return false;
        }
    }

    {
        D3D11_BUFFER_DESC desc{};
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.ByteWidth = sizeof(INDICES);
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA ib_data{};
        ib_data.pSysMem = INDICES;

        const HRESULT hr = device->CreateBuffer(
            &desc,
            &ib_data,
            this->chair_index_buffer.GetAddressOf()
        );
        if(FAILED(hr)) {
            return false;
        }
    }

    return true;
}

bool WallObject::make_mesh(ID3D11Device* const device) {
    if(!this->make_mesh_wall(device)) {
        return false;
    }

    if(!this->make_mesh_chair(device)) {
        return false;
    }

    return true;
}