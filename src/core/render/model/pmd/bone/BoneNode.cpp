#include "BoneNode.h"

BoneNode::BoneNode(const BindBone& bind_bone) :
    bind_bone(bind_bone),
    translate(DirectX::XMVectorSet(0, 0, 0, 1)),
    animation_rotate(DirectX::XMQuaternionIdentity()),
    ik_rotate(DirectX::XMQuaternionIdentity()),
    local(MMDMatrix::make_identity_matrix()),
    global(MMDMatrix::make_identity_matrix()) {
}

BoneNode::BoneNode(const BindBone& bind_bone, const std::shared_ptr<BoneNode>& parent_node) :
    bind_bone(bind_bone),
    parent(parent_node),
    translate(DirectX::XMVectorSet(0, 0, 0, 1)),
    animation_rotate(DirectX::XMQuaternionIdentity()),
    ik_rotate(DirectX::XMQuaternionIdentity()),
    local(MMDMatrix::make_identity_matrix()),
    global(MMDMatrix::make_identity_matrix()) {
}

std::shared_ptr<BoneNode> BoneNode::make(const BindBone& bind_bone) {
    return std::shared_ptr<BoneNode>(new BoneNode(bind_bone));
}

std::shared_ptr<BoneNode> BoneNode::make(
    const BindBone& bind_bone,
    const std::shared_ptr<BoneNode>& parent_node
) {
    const auto node = std::shared_ptr<BoneNode>(new BoneNode(bind_bone, parent_node));
    parent_node->children.push_back(node);
    return node;
}

void BoneNode::reset(void) {
    this->animation_rotate = DirectX::XMQuaternionIdentity();
    this->ik_rotate = DirectX::XMQuaternionIdentity();
    this->translate = DirectX::XMVectorSet(0, 0, 0, 1);
    this->local = this->bind_bone.local;
    this->global = this->bind_bone.global;
}

void BoneNode::update_local(void) {
    // 累積を合成
    const auto anim_translate = MMDMatrix::make_translation_from_vector(this->translate);
    const auto translate_matrix = this->bind_bone.local * anim_translate;
    const auto rotate = DirectX::XMQuaternionMultiply(
        this->animation_rotate,
        this->ik_rotate
    );
    const auto rotate_matrix = MMDMatrix::make_rotation_from_quaternion(rotate);

    this->local = MMDMatrix::make_transform_matrix(
        translate_matrix,
        rotate_matrix,
        MMDMatrix::make_identity_matrix() // MMDにスケールはない
    );
}

void BoneNode::update_global(void) {
    if(const auto parent_node = this->parent.lock()) {
        this->global = this->local * parent_node->global;
    } else {
        this->global = this->local;
    }

    this->update_children_global();
}

void BoneNode::update_children_global(void) {
    for(const auto& child : this->children) {
        if(const auto node = child.lock()) {
            if(const auto parent_node = node->parent.lock()) {
                node->global = node->local * parent_node->global;
            } else {
                node->global = node->local;
            }

            node->update_children_global();
        }
    }
}

void BoneNode::set_translate(const Vector4& translate) noexcept {
    this->translate = translate;
}

void BoneNode::set_animation_rotate(const Vector4& rotate) noexcept {
    this->animation_rotate = rotate;
}

void BoneNode::set_ik_rotate(const Vector4& rotate) noexcept {
    this->ik_rotate = DirectX::XMQuaternionNormalize(rotate);
}

void BoneNode::set_local(const MMDMatrix& local_matrix) noexcept {
    this->local = local_matrix;
}

void BoneNode::set_global(const MMDMatrix& global_matrix) noexcept {
    this->global = global_matrix;
}

const Vector4& BoneNode::get_translate(void) const noexcept {
    return this->translate;
}

const Vector4& BoneNode::get_animation_rotate(void) const noexcept {
    return this->animation_rotate;
}

const Vector4& BoneNode::get_ik_rotate(void) const noexcept {
    return this->ik_rotate;
}

const MMDMatrix& BoneNode::get_local(void) const noexcept {
    return this->local;
}

const MMDMatrix& BoneNode::get_global(void) const noexcept {
    return this->global;
}

const Vector4& BoneNode::get_global_position(void) const noexcept {
    return this->global.get_translation();
}