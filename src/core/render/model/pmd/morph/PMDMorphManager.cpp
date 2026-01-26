#include "../../../../utility/Maker.h"
#include "PMDMorphManager.h"

PMDMorphManager::PMDMorphManager(
    const std::shared_ptr<const PMDMorphs>& morphs,
    std::shared_ptr<std::vector<PMDVertexData>> vertices
) {
    this->vertices = vertices;

    std::vector<PMDMorphData> morph_list;
    for(const auto& morph : morphs->morphs) {
        this->morph_name_map[morph.name] = morph_list.size();

        morph_list.emplace_back(PMDMorphData{
            .skin_type = morph.skin_type,
            .vertices = morph.vertices,
            });
    }

    Maker::make_shared(this->morphs, std::move(morph_list));
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