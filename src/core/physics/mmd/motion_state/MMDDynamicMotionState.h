#pragma once
#include "MMDMotionState.h"
#include <memory>

class BoneNode;

class MMDDynamicMotionState : public MMDMotionState {
private:
    const std::shared_ptr<BoneNode> bone_node;
    const MMDMatrix offset;
    const MMDMatrix inverse_offset;
    btTransform transform;

public:
    explicit MMDDynamicMotionState(
        const std::shared_ptr<BoneNode>& bone_node,
        const MMDMatrix& offset
    );
    virtual ~MMDDynamicMotionState(void) noexcept = default;

    void getWorldTransform(btTransform& worldTrans) const override;
    void setWorldTransform(const btTransform& worldTrans) override;
    void reset(void) override;
    void reflect_global_transform(void) override;
};
