#pragma once
#include "MMDMotionState.h"
#include <memory>

class BoneNode;

class MMDDynamicAndBoneMergeMotionState : public MMDMotionState {
private:
    const std::shared_ptr<BoneNode> bone_node;
    const MMDMatrix inverse_offset;
    const MMDMatrix offset;
    btTransform	transform;
    const bool override_with_physics;

public:
    explicit MMDDynamicAndBoneMergeMotionState(
        const std::shared_ptr<BoneNode>& bone_node,
        const MMDMatrix& offset,
        const bool override_with_physics
    );
    virtual ~MMDDynamicAndBoneMergeMotionState(void) noexcept = default;

    void getWorldTransform(btTransform& worldTrans) const override;
    void setWorldTransform(const btTransform& worldTrans) override;
    void reset(void) override;
    void reflect_global_transform(void) override;
};
