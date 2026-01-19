#include "../../../../utility/Algorithm.h"
#include "BoneKeyFrameCursor.h"

BoneKeyFrameCursor::BoneKeyFrameCursor(
    const std::shared_ptr<FrameManager>& frame_manager,
    std::vector<BoneKeyFrame>&& key_frame_list
) : KeyFrameCursor<BoneKeyFrame>(
    frame_manager,
    BoneKeyFrameCursor::sort(std::move(key_frame_list)),
    [](const BoneKeyFrame& key_frame) {
        return key_frame.frame;
    }
) {
}

DirectX::XMVECTOR BoneKeyFrameCursor::get_rotate(void) const {
    const auto& opt_previous = this->get_previous_key_frame();
    const auto& opt_current = this->get_current_key_frame();

    if(!opt_current.has_value()) {
        // TODO: log
        return DirectX::XMQuaternionIdentity();
    }

    const auto& current = opt_current.value();
    const auto& current_frame = current.frame;
    const auto& current_rotate = current.rotation;
    const DirectX::XMFLOAT2 p1 = DirectX::XMFLOAT2(
        static_cast<float>(current.interpolation[3]) / 127.0f,
        static_cast<float>(current.interpolation[7]) / 127.0f
    );
    const DirectX::XMFLOAT2 p2 = DirectX::XMFLOAT2(
        static_cast<float>(current.interpolation[11]) / 127.0f,
        static_cast<float>(current.interpolation[15]) / 127.0f
    );

    const auto& has_value = opt_previous.has_value();
    const auto& previous_frame = has_value ? opt_previous.value().frame : 0;
    const auto& previous_rotation = has_value ? opt_previous.value().rotation : DirectX::XMQuaternionIdentity();

    if(previous_frame == current_frame) {
        return DirectX::XMQuaternionIdentity();
    }

    const float frame_ratio = this->frame_manager->progress(
        previous_frame,
        current_frame
    );
    const float t = y_bezier(frame_ratio, p1, p2, 100); // TODO: config
    return slerp_quaternion(
        previous_rotation,
        current_rotate,
        t
    );
}

DirectX::XMVECTOR BoneKeyFrameCursor::get_translate(void) const {
    const auto& opt_previous = this->get_previous_key_frame();
    const auto& opt_current = this->get_current_key_frame();

    if(!opt_current.has_value()) {
        // TODO: log
        return DirectX::XMVectorZero();
    }

    const auto& current = opt_current.value();
    const auto& previous_index = opt_previous.has_value() ? opt_previous.value().frame : 0;
    const auto& previous_rotate = opt_previous.has_value() ? opt_previous.value().rotation : DirectX::XMQuaternionIdentity();
    const float diff = static_cast<float>(current.frame) - previous_index;
    if(diff == 0) {
        return current.translation;
    }

    return current.translation;
}