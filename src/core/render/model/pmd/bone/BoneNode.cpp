#include "BoneNode.h"

BoneNode::BoneNode(void) :
    angle(0.0f),
    rotate(DirectX::XMQuaternionIdentity()),
    local(DirectX::XMMatrixIdentity()),
    global(DirectX::XMMatrixIdentity()) {
}

void BoneNode::update_local(void) {
}

void BoneNode::update_global(void) {
}