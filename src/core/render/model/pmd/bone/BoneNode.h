#pragma once
#include "BindBone.h"
#include <DirectXMath.h>
#include <memory>
#include <vector>

class BoneNode {
protected:
    std::vector<std::weak_ptr<BoneNode>> children;
    DirectX::XMVECTOR rotate; // 四元数
    DirectX::XMMATRIX local;
    DirectX::XMMATRIX global;

public:
    const std::weak_ptr<const BoneNode> parent;
    const BindBone bind_bone;

private:
    explicit BoneNode(void) = delete;

protected:
    explicit BoneNode(const BindBone& bind_bone);
    explicit BoneNode(
        const BindBone& bind_bone,
        const std::shared_ptr<BoneNode>& parent_node
    );

public:
    static std::shared_ptr<BoneNode> make(const BindBone& bind_bone);
    static std::shared_ptr<BoneNode> make(
        const BindBone& bind_bone,
        const std::shared_ptr<BoneNode>& parent_node
    );

    void reset(void);

    void update_local(void);

    void update_global(void);

    void set_rotate(const DirectX::XMVECTOR& rotate) noexcept;
    void set_local(const DirectX::XMMATRIX& local_matrix) noexcept;
    void set_global(const DirectX::XMMATRIX& global_matrix) noexcept;
    const DirectX::XMVECTOR& get_rotate(void) const noexcept;
    const DirectX::XMMATRIX& get_local(void) const noexcept;
    const DirectX::XMMATRIX& get_global(void) const noexcept;

    const DirectX::XMVECTOR& get_global_position(void) const noexcept;
};
