#pragma once
#include "vmd_file_struct.h"
#include <array>
#include <cstdint>
#include <vector>

namespace enishi::assets_system {
    constexpr float VMD_FPS = 30.0f;

    struct VMDData {
        std::array<char, 20> name;
        std::vector<VMDBoneKeyFrame> bone_key_frames;
        std::vector<VMDMorphKeyFrame> morph_key_frames;
        std::vector<VMDCameraKeyframe> camera_key_frames;
        std::vector<VMDLight> light_key_frames;
        std::vector<VMDShadow> shadow_key_frames;
        std::vector<VMDIKInfo> ik_infos;
        std::vector<VMDIK> iks;
    };
} // namespace enishi::assets_system