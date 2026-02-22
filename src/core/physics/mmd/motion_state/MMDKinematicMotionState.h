#pragma once
#include "../../../math/WrappedMatrix.h"
#include "MMDMotionState.h"
#include <memory>

class BoneNode;

class MMDKinematicMotionState : public MMDMotionState {
private:
    std::shared_ptr<BoneNode> bone_node;
    MMDMatrix offset;
    btTransform	transform;

public:
    explicit MMDKinematicMotionState(
        const std::shared_ptr<BoneNode>& bone_node,
        const MMDMatrix& offset
    );
    virtual ~MMDKinematicMotionState(void) noexcept = default;

    void getWorldTransform(btTransform& worldTrans) const override;
    void setWorldTransform(const btTransform& worldTrans) override;
    void reset(void) override;
    void reflect_global_transform(void) override;
};
