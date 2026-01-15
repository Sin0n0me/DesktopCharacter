#include "MorphKeyFrameManager.h"

MorphKeyFrameManager::MorphKeyFrameManager(const std::vector<VMDMorphKeyframe>& morph_key_frames) {
    //this->vertices = vertices;
}

void MorphKeyFrameManager::apply_morph(const uint32_t& frame) {
    for(const auto& morph_index : this->apply_morphs) {
        /*
        const auto weight_it = morph_weights.find(morph_data.name);
        if(weight_it == morph_weights.end()) {
            continue;
        }

        const float weight = weight_it->second;
        if(weight <= 0.0f) {
            continue;
        }

        for(const vertex_offset& offset : morph_data.offsets) {
            const std::uint32_t index = offset.vertex_index;
            if(index >= vertex_count) {
                continue;
            }

            auto& vertices = this->vertices->at(index);
            vertices.position[0] += offset.offset_x * weight;
            vertices.position[1] += offset.offset_y * weight;
            vertices.position[2] += offset.offset_z * weight;
        }
        */
    }
}