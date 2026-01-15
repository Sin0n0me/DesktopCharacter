#include "../../../../utility/Maker.h"
#include "PMDMorphManager.h"

PMDMorphManager::PMDMorphManager(
    const std::vector<PMDMorph>& morphs,
    std::shared_ptr<std::vector<PMDVertexData>> vertices
) {
    this->vertices = vertices;

    std::vector<PMDMorphData> morph_list;
    for(const auto& morph : morphs) {
        this->morph_name_map[morph.name] = morph_list.size();

        morph_list.emplace_back(PMDMorphData{
            .skin_type = morph.skin_type,
            .vertices = morph.vertices,
            });
    }

    Maker::make_shared(this->morphs, std::move(morph_list));
}

bool PMDMorphManager::init(ID3D11Device* const device) {
    return true;
}

void PMDMorphManager::render_update(ID3D11DeviceContext* const context) {
}

void PMDMorphManager::render(
    ID3D11DeviceContext* const context,
    const ShaderBindingSlots* slots
) const {
}