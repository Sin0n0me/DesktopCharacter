#include "../../../log/Logger.h"
#include "../../../render/model/pmd/bone/BoneNode.h"
#include "../../TransformConverter.h"
#include "MMDKinematicMotionState.h"

MMDKinematicMotionState::MMDKinematicMotionState(
    const std::shared_ptr<BoneNode>& bone_node,
    const MMDMatrix& offset
) :
    bone_node(bone_node),
    offset(offset) {
}

void MMDKinematicMotionState::getWorldTransform(btTransform& worldTrans) const {
    // mmdの世界でオフセット適用後にbulletの世界に変換
    const MMDMatrix global = this->bone_node->get_global();
    const MMDMatrix offset_matrix = global * this->offset;
    worldTrans = matrix_to_transform(offset_matrix.inverse_z());
}

void MMDKinematicMotionState::setWorldTransform(const btTransform& worldTrans) {
}

void MMDKinematicMotionState::reset(void) {
}

void MMDKinematicMotionState::reflect_global_transform(void) {
}