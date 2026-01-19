#pragma once
#include "../PMDFileStruct.h"
#include "../vertex/PMDVertexData.h"
#include "IMorphAccessor.h"
#include "Morph.h"
#include <memory>
#include <string>
#include <unordered_map>

class PMDMorphManager : public IMorphAccessor {
private:
    std::shared_ptr<std::vector<PMDVertexData>> vertices;    // 更新用
    std::shared_ptr<const std::vector<PMDMorphData>> morphs; // 参照用
    std::unordered_map<std::string, uint32_t> morph_name_map;

public:

    explicit PMDMorphManager(
        const std::vector<PMDMorph>& morphs,
        std::shared_ptr<std::vector<PMDVertexData>> vertices
    );

    std::shared_ptr<std::vector<PMDVertexData>> get_mutable_vertices(void) const noexcept override;
    std::shared_ptr<const std::vector<PMDMorphData>> get_morphs(void) const noexcept override;
    uint32_t get_morph_index(const std::string& name) const noexcept override;
};
