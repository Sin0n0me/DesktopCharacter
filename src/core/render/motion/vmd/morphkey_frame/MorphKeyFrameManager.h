#pragma once

#include "../../../model/pmd/morph/Morph.h"
#include "../../../model/pmd/vertex/PMDVertexData.h"
#include "../VMDFileStruct.h"
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

class IMorphAccessor;

class MorphKeyFrameManager {
private:
    struct MorphProfile {
        float weight;
    };

private:

    std::shared_ptr<IMorphAccessor> morph_accessor; // 更新用
    std::shared_ptr<std::vector<PMDVertexData>> vertices; // 更新用
    std::vector<VMDMorphKeyframe> morphs;
    std::unordered_map<uint32_t, float> morph_map;
    std::set<uint32_t> apply_morphs;
    uint32_t previous_frame;

    void set_frame(const uint32_t& frame);

public:

    explicit MorphKeyFrameManager(
        const std::vector<VMDMorphKeyframe>& morph_key_frames
    );

    void apply_morph(

        const uint32_t& frame
    );

    bool resolve_morphs(
        const std::vector<VMDMorphKeyframe>& morphs,
        const std::unordered_map<std::string, PMDMorphData>& morph_name_map
    );
};
