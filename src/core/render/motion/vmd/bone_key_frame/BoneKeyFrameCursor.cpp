#include "../../../../utility/Algorithm.h"
#include "BoneKeyFrameCursor.h"

// TODO: DirectXMathからラッパーへの変更

BoneKeyFrameCursor::BoneKeyFrameCursor(
    const std::shared_ptr<KeyFrameTimer>& frame_manager,
    std::vector<BoneKeyFrame>&& key_frame_list
) : KeyFrameCursor<BoneKeyFrame>(
    frame_manager,
    BoneKeyFrameCursor::sort(std::move(key_frame_list)),
    [](const BoneKeyFrame& key_frame) {
        return key_frame.frame;
    }
) {
}

float BoneKeyFrameCursor::get_frame_ratio(void) const noexcept {
    const auto& opt_previous = this->get_previous_key_frame();
    const auto& opt_current = this->get_current_key_frame();
    if(!opt_current.has_value()) {
        return 0.0f;
    }

    const auto& current = opt_current.value();
    const auto& current_frame = current.frame;
    const auto& has_value = opt_previous.has_value();
    const auto& previous_frame = has_value ? opt_previous.value().frame : 0;

    return this->frame_manager->progress(
        previous_frame,
        current_frame
    );
}

DirectX::XMVECTOR BoneKeyFrameCursor::get_rotate(void) const {
    const auto& opt_previous = this->get_previous_key_frame();
    const auto& opt_current = this->get_current_key_frame();

    if(!opt_current.has_value()) {
        // TODO: log
        return DirectX::XMQuaternionIdentity();
    }

    const auto& current = opt_current.value();
    const auto& current_rotate = current.rotation;
    const auto& has_value = opt_previous.has_value();
    const auto& previous_rotation = has_value ?
        opt_previous.value().rotation :
        DirectX::XMQuaternionIdentity();
    const float frame_ratio = this->get_frame_ratio();

    const auto& bezier = current.bezier.rotate_bezier;
    const float rotate_x = bezier.find_bezier_x(frame_ratio);
    const float rotate_y = bezier.eval_y(rotate_x);

    return DirectX::XMQuaternionSlerp(
        previous_rotation,
        current_rotate,
        rotate_y
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
    const auto& previous_translate = opt_previous.has_value() ?
        opt_previous.value().translation :
        DirectX::XMVectorSet(0, 0, 0, 0);

    const float frame_ratio = this->get_frame_ratio();
    const auto& bezier = current.bezier;
    const float tx_x = bezier.translate_x_bezier.find_bezier_x(frame_ratio);
    const float ty_x = bezier.translate_y_bezier.find_bezier_x(frame_ratio);
    const float tz_x = bezier.translate_z_bezier.find_bezier_x(frame_ratio);
    const float tx_y = bezier.translate_x_bezier.eval_y(tx_x);
    const float ty_y = bezier.translate_y_bezier.eval_y(ty_x);
    const float tz_y = bezier.translate_z_bezier.eval_y(tz_x);
    const auto interpolation = DirectX::XMVectorSet(tx_y, ty_y, tz_y, 1.0);

    return DirectX::XMVectorAdd(
        DirectX::XMVectorMultiply(
            previous_translate,
            DirectX::XMVectorSubtract(
                DirectX::XMVectorSplatOne(),
                interpolation
            )
        ),
        DirectX::XMVectorMultiply(
            current.translation,
            interpolation
        )
    );
}