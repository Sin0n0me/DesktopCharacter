#include "PMDMorphManager.h"

PMDMorphManager::PMDMorphManager(
    const std::shared_ptr<const PMDMorphs>& morphs,
    std::shared_ptr<std::vector<PMDVertexData>> vertices
) :
    vertices(vertices),
    morphs(new std::vector<PMDMorphData>(morphs->size)) {
    for(auto i = 0; i < morphs->size; ++i) {
        const auto& morph = morphs->morphs.at(i);
        this->morph_name_map[morph.name] = i;

        auto& data = this->morphs->at(i);
        data.skin_type = morph.skin_type;
        data.vertices = morph.vertices;
    }
}

std::shared_ptr<std::vector<PMDVertexData>> PMDMorphManager::get_mutable_vertices(void) const noexcept {
    return this->vertices;
}

std::shared_ptr<const std::vector<PMDMorphData>> PMDMorphManager::get_morphs(void) const noexcept {
    return this->morphs;
}

std::optional<uint32_t> PMDMorphManager::get_morph_index(const std::string& name) const noexcept {
    const auto& iter = this->morph_name_map.find(name);
    if(iter == this->morph_name_map.end()) {
        return std::nullopt;
    }

    return iter->second;
}