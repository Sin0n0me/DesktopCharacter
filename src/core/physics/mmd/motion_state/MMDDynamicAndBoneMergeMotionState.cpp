#include "../../../log/Logger.h"
#include "../../../render/model/pmd/bone/BoneNode.h"
#include "../../TransformConverter.h"
#include "MMDDynamicAndBoneMergeMotionState.h"

MMDDynamicAndBoneMergeMotionState::MMDDynamicAndBoneMergeMotionState(
    const std::shared_ptr<BoneNode>& bone_node,
    const MMDMatrix& offset
) :
    bone_node(bone_node),
    offset(offset),
    inverse_offset(offset.inverse()) {
    this->reset();
}

void MMDDynamicAndBoneMergeMotionState::getWorldTransform(btTransform& worldTrans) const {
    worldTrans = this->transform;
}

void MMDDynamicAndBoneMergeMotionState::setWorldTransform(const btTransform& worldTrans) {
    this->transform = worldTrans;
}

void MMDDynamicAndBoneMergeMotionState::reset(void) {
    // mmdの世界でオフセット適用後にbulletの世界に変換
    const MMDMatrix& offset_matrix = this->offset * this->bone_node->get_global();
    this->transform = matrix_to_transform(offset_matrix.to_bullet());
}

void MMDDynamicAndBoneMergeMotionState::reflect_global_transform(void) {
    // MMDの世界に変換
    const MMDMatrix matrix = transform_to_matrix(this->transform).to_mmd();

    // 中心とのoffsetが掛かっているので
    // offsetの逆行列を掛けることでボーン空間に戻す
    MMDMatrix global = this->inverse_offset * matrix;
    global.set_translation(this->bone_node->get_global_position());

    this->bone_node->set_global(global);
    this->bone_node->update_children_global();
}