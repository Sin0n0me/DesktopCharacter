#include "../../../model/pmd/morph/IMorphAccessor.h"
#include "MorphKeyFrameCursor.h"
#include <cmath>

MorphKeyFrameCursor::MorphKeyFrameCursor(
    const std::shared_ptr<KeyFrameTimer>& frame_manager,
    std::vector<MorphKeyFrame>&& key_frame_list
) : KeyFrameCursor<MorphKeyFrame>(
    frame_manager,
    MorphKeyFrameCursor::sort(std::move(key_frame_list)),
    [](const MorphKeyFrame& key_frame) {
        return key_frame.frame;
    }
) {
}

void MorphKeyFrameCursor::apply_offset(
    const uint32_t index,
    const std::shared_ptr<IMorphAccessor>& morph_accessor
) const {
    const auto opt_current = this->get_current_key_frame();
    if(!opt_current.has_value()) {
        return;
    }
    const auto& current = opt_current.value();
    const auto opt_previous = this->get_previous_key_frame();

    // 線形補完
    const auto manager = this->frame_manager.lock();
    if(!bool(manager)) {
        return;
    }
    const auto current_frame = current.get().frame;
    const auto previous_frame = opt_previous.has_value() ? opt_previous.value().get().frame : 0;
    const auto& current_weight = current.get().weight;
    const auto& previous_weight = opt_previous.has_value() ? opt_previous.value().get().weight : 0.0f;
    const float weight = std::lerp(
        previous_weight,
        current_weight,
        manager->progress(
            previous_frame,
            current_frame
        )
    );

    // weight が0なら後続の処理は計算するだけ無駄
    if(weight == 0.0) {
        return;
    }

    const auto& mutable_vertices = morph_accessor->get_mutable_vertices();
    const auto& base_morph = morph_accessor->get_morphs()->at(0);
    const auto& morphs = morph_accessor->get_morphs()->at(index);
    for(const auto& vertex_data : morphs.vertices) {
        const auto& offset = vertex_data.position;                    // ベースとのオフセット
        const auto& base = base_morph.vertices.at(vertex_data.index); // 0以外の場合はベースのインデックス
        auto& vertex = mutable_vertices->at(base.index);              // baseのインデックスがモデルの頂点インデックス
        vertex.position[0] = base.position[0] + offset[0] * weight;
        vertex.position[1] = base.position[1] + offset[1] * weight;
        vertex.position[2] = base.position[2] + offset[2] * weight;
    }
}