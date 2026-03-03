#include "../../../log/Logger.h"
#include "../../../render/model/pmd/bone/BoneNode.h"
#include "../../TransformConverter.h"
#include "MMDDynamicMotionState.h"

MMDDynamicMotionState::MMDDynamicMotionState(
    const std::shared_ptr<BoneNode>& bone_node,
    const MMDMatrix& offset
) :
    bone_node(bone_node),
    offset(offset),
    inverse_offset(offset.inverse()) {
    this->reset();
}

void MMDDynamicMotionState::getWorldTransform(btTransform& worldTrans) const {
    worldTrans = this->transform;
}

void MMDDynamicMotionState::setWorldTransform(const btTransform& worldTrans) {
    this->transform = worldTrans;
}

void MMDDynamicMotionState::reset(void) {
    // mmdの世界からbulletの世界に変換しオフセット適用
    const MMDMatrix global = this->bone_node->get_global();
    const MMDMatrix offset_matrix = this->offset * global;
    this->transform = matrix_to_transform(offset_matrix);
}

void MMDDynamicMotionState::reflect_global_transform(void) {
    // 行優先計算
    // 中心とのoffsetが掛かっているので
    // offsetの逆行列を掛けることでボーン空間に戻す
    const MMDMatrix global = this->inverse_offset * transform_to_matrix(this->transform);

    // MMDの世界に変換
    this->bone_node->set_global(global);
    this->bone_node->update_children_global();
}