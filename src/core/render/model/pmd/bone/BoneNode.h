#pragma once
#include "BindBone.h"
#include <memory>
#include <vector>

class BoneNode {
protected:
    std::vector<std::weak_ptr<BoneNode>> children;
    Vector4 translate;
    Vector4 animation_rotate; // 四元数
    Vector4 ik_rotate;        // 四元数
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

    void set_translate(const Vector4& translate) noexcept;
    void set_animation_rotate(const Vector4& rotate) noexcept;
    void set_ik_rotate(const Vector4& rotate) noexcept;
    void set_local(const MMDMatrix& local_matrix) noexcept;
    void set_global(const MMDMatrix& global_matrix) noexcept;
    const Vector4& get_translate(void) const noexcept;
    const Vector4& get_animation_rotate(void) const noexcept;
    const Vector4& get_ik_rotate(void) const noexcept;
    const MMDMatrix& get_local(void) const noexcept;
    const MMDMatrix& get_global(void) const noexcept;

    const Vector4& get_global_position(void) const noexcept;
};
