#include "../../../../utility/Algorithm.h"
#include "BoneKeyFrameCursor.h"
#include <algorithm>

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
    if(!opt_previous.has_value()) {
        return 1.0f;
    }

    const auto manager = this->frame_manager.lock();
    if(!bool(manager)) {
        return 1.0f;
    }

    const auto& current = opt_current.value();
    const auto& current_frame = current.get().frame;
    const auto& previous_frame = opt_previous.value().get().frame;

    return manager->progress(
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
    if(!opt_previous.has_value()) {
        return current.get().rotation;
    }

    const auto& current_rotation = current.get().rotation;
    const auto& previous_rotation = opt_previous.value().get().rotation;
    const float frame_ratio = this->get_frame_ratio();

    // 動きを滑らかにするために球面線形補間
    const auto& bezier = current.get().bezier.rotate_bezier;
    const float rotate_x = bezier.find_bezier_x(frame_ratio);
    const float rotate_y = bezier.eval_y(rotate_x);
    const auto rotation = DirectX::XMQuaternionSlerp(
        previous_rotation,
        current_rotation,
        rotate_y
    );

    return DirectX::XMQuaternionNormalize(rotation);
}

DirectX::XMVECTOR BoneKeyFrameCursor::get_translate(void) const {
    const auto& opt_previous = this->get_previous_key_frame();
    const auto& opt_current = this->get_current_key_frame();

    if(!opt_current.has_value()) {
        // TODO: log
        return DirectX::XMVectorZero();
    }
    const auto& current = opt_current.value();
    if(!opt_previous.has_value()) {
        return current.get().translation;
    }

    const auto& current_translate = current.get().translation;
    const auto& previous_translate = opt_previous.value().get().translation;

    const float frame_ratio = this->get_frame_ratio();
    const auto& bezier = current.get().bezier;
    const float tx_x = bezier.translate_x_bezier.find_bezier_x(frame_ratio);
    const float ty_x = bezier.translate_y_bezier.find_bezier_x(frame_ratio);
    const float tz_x = bezier.translate_z_bezier.find_bezier_x(frame_ratio);
    const float tx_y = bezier.translate_x_bezier.eval_y(tx_x);
    const float ty_y = bezier.translate_y_bezier.eval_y(ty_x);
    const float tz_y = bezier.translate_z_bezier.eval_y(tz_x);
    const auto interpolation = DirectX::XMVectorSet(tx_y, ty_y, tz_y, 0.0f);

    // 平行移動成分の線形補完
    // T = pT * (1 - t) + cT * t
    const auto it = DirectX::XMVectorSubtract(
        DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
        interpolation
    );
    const auto previous_translate_ = DirectX::XMVectorMultiply(
        previous_translate,
        it
    );
    const auto current_translation_ = DirectX::XMVectorMultiply(
        current_translate,
        interpolation
    );

    return DirectX::XMVectorAdd(
        previous_translate_,
        current_translation_
    );
}