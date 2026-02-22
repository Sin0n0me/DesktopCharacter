#pragma once
#include "MMDMotionState.h"
#include <memory>

class BoneNode;

class MMDDynamicAndBoneMergeMotionState : public MMDMotionState {
private:
    std::shared_ptr<BoneNode> bone_node;
    MMDMatrix inverse_offset;
    MMDMatrix offset;
    btTransform	transform;

public:
    explicit MMDDynamicAndBoneMergeMotionState(
        const std::shared_ptr<BoneNode>& bone_node,
        const MMDMatrix& offset
    );
    virtual ~MMDDynamicAndBoneMergeMotionState(void) noexcept = default;

    void getWorldTransform(btTransform& worldTrans) const override;
    void setWorldTransform(const btTransform& worldTrans) override;
    void reset(void) override;
    void reflect_global_transform(void) override;
};
