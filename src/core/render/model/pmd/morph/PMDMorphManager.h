#pragma once
#include "../../../../object/IObjectRenderer.h"
#include "../PMDFileStruct.h"
#include "../vertex/PMDVertexData.h"
#include "IMorphAccessor.h"
#include "Morph.h"
#include <memory>
#include <string>
#include <unordered_map>

class PMDMorphManager : public IObjectRenderer, public IMorphAccessor {
private:
    std::shared_ptr<std::vector<PMDVertexData>> vertices;    // 更新用
    std::shared_ptr<const std::vector<PMDMorphData>> morphs; // 参照用
    std::unordered_map<std::string, uint32_t> morph_name_map;

public:

    explicit PMDMorphManager(
        const std::vector<PMDMorph>& morphs,
        std::shared_ptr<std::vector<PMDVertexData>> vertices
    );

    bool init(ID3D11Device* const device) override;
    void render_update(ID3D11DeviceContext* const context) override;
    void render(ID3D11DeviceContext* const context, const ShaderBindingSlots* slots) const override;
};
