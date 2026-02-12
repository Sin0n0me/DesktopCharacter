#include "BoneNode.h"

BoneNode::BoneNode(const BindBone& bind_bone) :
    bind_bone(bind_bone),
    rotate(DirectX::XMQuaternionIdentity()),
    local(MMDMatrix::make_identity_matrix()),
    global(MMDMatrix::make_identity_matrix()) {
}

BoneNode::BoneNode(const BindBone& bind_bone, const std::shared_ptr<BoneNode>& parent_node) :
    bind_bone(bind_bone),
    parent(parent_node),
    rotate(DirectX::XMQuaternionIdentity()),
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
    this->local = this->bind_bone.local;
    this->global = this->bind_bone.global;
}

void BoneNode::update_local(void) {
    const auto& rotate_matrix = MMDMatrix::make_rotation_from_quaternion(
        this->rotate
    );
    this->local = rotate_matrix * this->bind_bone.local;
}

void BoneNode::update_global(void) {
    if(const auto parent_node = this->parent.lock()) {
        this->global = this->local * parent_node->global;
    } else {
        this->global = this->local;
    }

    for(const auto& child : this->children) {
        const auto node = child.lock();
        if(!bool(node)) {
            continue;
        }

        if(const auto parent_node = node->parent.lock()) {
            node->global = node->local * parent_node->global;
        } else {
            node->global = node->local;
        }

        node->update_global();
    }
}

void BoneNode::set_rotate(const DirectX::XMVECTOR& rotate) noexcept {
    this->rotate = DirectX::XMQuaternionNormalize(rotate);
}

void BoneNode::set_local(const MMDMatrix& local_matrix) noexcept {
    this->local = local_matrix;
}

void BoneNode::set_global(const MMDMatrix& global_matrix) noexcept {
    this->global = global_matrix;
}

const DirectX::XMVECTOR& BoneNode::get_rotate(void) const noexcept {
    return this->rotate;
}

const MMDMatrix& BoneNode::get_local(void) const noexcept {
    return this->local;
}

const MMDMatrix& BoneNode::get_global(void) const noexcept {
    return this->global;
}

const DirectX::XMVECTOR& BoneNode::get_global_position(void) const noexcept {
    return this->global.get_translation();
}