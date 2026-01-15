#include "../../../utility/BinaryReader.h"
#include "VMDLoader.h"

VMDLoader::VMDLoader(const std::filesystem::path& path) noexcept : path(path) {
}

bool VMDLoader::load_vmd_header(BinaryReader& binary_reader) {
    VMDHeader header{};
    binary_reader.read_to(&header);

    if(std::strncmp(header.header, "Vocaloid Motion Data", 21) == 0) {
        return true;
    }
    if(std::strncmp(header.header, "Vocaloid Motion Data 0002", 26) == 0) {
        return true;
    }

    return false;
}

bool VMDLoader::load_vmd_bone_key_frame(BinaryReader& binary_reader) {
    uint32_t count = 0;
    if(!binary_reader.read_to(&count)) {
        return false;
    }

    if(!binary_reader.read_to_vec(this->bone_key_frames, count)) {
        return false;
    }

    return true;
}

bool VMDLoader::load_vmd_morph_key_frame(BinaryReader& binary_reader) {
    uint32_t count = 0;
    if(!binary_reader.read_to(&count)) {
        return false;
    }

    if(!binary_reader.read_to_vec(this->morph_key_frames, count)) {
        return false;
    }

    return true;
}

bool VMDLoader::load_vmd_camera(BinaryReader& binary_reader) {
    uint32_t count = 0;
    if(!binary_reader.read_to(&count)) {
        return false;
    }

    // 今のところ使用しない
    std::vector<VMDCameraKeyframe> camera;
    if(!binary_reader.read_to_vec(camera, count)) {
        return false;
    }

    return true;
}

bool VMDLoader::load_vmd_light(BinaryReader& binary_reader) {
    uint32_t count = 0;
    if(!binary_reader.read_to(&count)) {
        return false;
    }

    // 今のところ使用しない
    std::vector<VMDLight> camera;
    if(!binary_reader.read_to_vec(camera, count)) {
        return false;
    }

    return true;
}

bool VMDLoader::load_vmd_shadow(BinaryReader& binary_reader) {
    uint32_t count = 0;
    if(!binary_reader.read_to(&count)) {
        return false;
    }

    // 今のところ使用しない
    std::vector<VMDShadow> camera;
    if(!binary_reader.read_to_vec(camera, count)) {
        return false;
    }

    return true;
}

bool VMDLoader::load_vmd_ik(BinaryReader& binary_reader) {
    uint32_t count = 0;
    if(!binary_reader.read_to(&count)) {
        return false;
    }

    this->iks.resize(count);
    for(decltype(count) i = 0; i < count; i++) {
        auto& ik = this->iks[i];
        //
        const bool result = binary_reader.read(
            &ik,
            sizeof(VMDIK) - sizeof(VMDIK::ik_infos)
        );
        if(!result) {
            return false;
        }

        if(!binary_reader.read_to_vec(ik.ik_infos, ik.count)) {
            return false;
        }
    }

    return true;
}

bool VMDLoader::load_vmd(void) {
    auto reader = BinaryReader::make_reader(this->path);
    if(!reader.has_value()) {
        return false;
    }
    auto& binary_reader = reader.value();

    // ヘッダ読み込み
    if(!this->load_vmd_header(binary_reader)) {
        return false;
    }

    // ボーンキーフレームの読み込み
    if(!this->load_vmd_bone_key_frame(binary_reader)) {
        return false;
    }

    // モーフの読み込み
    if(!this->load_vmd_morph_key_frame(binary_reader)) {
        return false;
    }

    // カメラの読み込み
    if(!this->load_vmd_camera(binary_reader)) {
        return false;
    }

    // ライトの読み込み
    if(!this->load_vmd_light(binary_reader)) {
        return false;
    }

    // 影の読み込み
    if(!this->load_vmd_shadow(binary_reader)) {
        return false;
    }

    // IKの読み込み
    if(!this->load_vmd_ik(binary_reader)) {
        return false;
    }

    return true;
}

const std::vector<VMDBoneKeyFrame>& VMDLoader::get_bone_key_frames(void) const noexcept {
    return this->bone_key_frames;
}

const std::vector<VMDMorphKeyframe>& VMDLoader::get_morph_key_frames(void) const noexcept {
    return this->morph_key_frames;
}

const std::vector<VMDIK>& VMDLoader::get_iks(void) const noexcept {
    return this->iks;
}