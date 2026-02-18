#pragma once
#include "BindBone.h"
#include <DirectXMath.h>
#include <memory>
#include <vector>

class BoneNode {
protected:
    std::vector<std::weak_ptr<BoneNode>> children;
    Vector4 rotate; // 四元数
    MMDMatrix local;
    MMDMatrix global;

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

    void update_children_global(void);

    void set_rotate(const DirectX::XMVECTOR& rotate) noexcept;
    void set_local(const MMDMatrix& local_matrix) noexcept;
    void set_global(const MMDMatrix& global_matrix) noexcept;
    const DirectX::XMVECTOR& get_rotate(void) const noexcept;
    const MMDMatrix& get_local(void) const noexcept;
    const MMDMatrix& get_global(void) const noexcept;

    const DirectX::XMVECTOR& get_global_position(void) const noexcept;
};
