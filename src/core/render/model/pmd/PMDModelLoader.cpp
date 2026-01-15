#include "../../../utility/BinaryReader.h"
#include "PMDModelLoader.h"
#include <optional>

PMDModelLoader::PMDModelLoader(const std::filesystem::path& path) noexcept : path(path) {
}

// 先頭から順に
// ヘッダー(284Byte)
// 頂点数(4Byte)
// 頂点データ(40Byte * 頂点数)
// インデックス数(4Byte)
// インデックスデータ(2Byte * インデックス数)
// マテリアル数(4Byte)
// マテリアルデータ(70Byte)
// ボーン数(2Byte)
// ボーンデータ
// IK数
// IKデータ
bool PMDModelLoader::load_pmd() {
    auto reader = BinaryReader::make_reader(path);
    if(!reader.has_value()) {
        return false;
    }
    auto& binary_reader = reader.value();

    // モデルデータの読み込み開始

    // ヘッダ読み込み
    if(!this->load_pmd_header(binary_reader)) {
        return false;
    }

    // vertices
    if(!this->load_pmd_vertices(binary_reader)) {
        return false;
    }

    // indices
    if(!this->load_pmd_indices(binary_reader)) {
        return false;
    }

    // materials
    if(!this->load_pmd_materials(binary_reader)) {
        return false;
    }

    // bone_matrices
    if(!this->load_pmd_bones(binary_reader)) {
        return false;
    }

    // IK
    if(!this->load_pmd_iks(binary_reader)) {
        return false;
    }

    // モーフ(表情)
    if(!this->load_pmd_morph(binary_reader)) {
        return false;
    }

    // 英名対応
    {
    }

    // トゥーンテクスチャリスト
    {
    }

    // 物理剛体
    {
    }

    return true;
}

bool PMDModelLoader::load_pmd_header(BinaryReader& binary_reader) {
    PMDHeader header{};
    binary_reader.read_to(&header);
    if(std::strncmp(header.magic, "Pmd", 3) != 0) {
        return false;
    }

    return true;
}

bool PMDModelLoader::load_pmd_vertices(BinaryReader& binary_reader) {
    uint32_t vertex_count{};
    if(!binary_reader.read_to(&vertex_count)) {
        return false;
    }

    this->vertices.resize(vertex_count);
    if(!binary_reader.read_to_vec(this->vertices, vertex_count)) {
        return false;
    }

    return true;
}

bool PMDModelLoader::load_pmd_indices(BinaryReader& binary_reader) {
    uint32_t index_count{};
    if(!binary_reader.read_to(&index_count)) {
        return false;
    }

    this->indices.resize(index_count);
    if(!binary_reader.read_to_vec(this->indices, index_count)) {
        return false;
    }

    return true;
}

bool PMDModelLoader::load_pmd_materials(BinaryReader& binary_reader) {
    uint32_t material_count{};
    if(!binary_reader.read_to(&material_count)) {
        return false;
    }

    this->materials.resize(material_count);
    if(!binary_reader.read_to_vec(this->materials, material_count)) {
        return false;
    }

    return true;
}

bool PMDModelLoader::load_pmd_bones(BinaryReader& binary_reader) {
    uint16_t bone_count{};
    if(!binary_reader.read_to(&bone_count)) {
        return false;
    }

    if(!binary_reader.read_to_vec(this->bones, bone_count)) {
        return false;
    }

    return true;
}

bool PMDModelLoader::load_pmd_iks(BinaryReader& binary_reader) {
    uint16_t ik_count{};
    if(!binary_reader.read_to(&ik_count)) {
        return false;
    }

    this->iks.resize(ik_count);
    for(int i = 0; i < ik_count; i++) {
        // 可変部分であるchainの部分だけは後で読み込むように
        // 先頭11Byteだけ読み込み
        auto& ik = this->iks[i];
        if(!binary_reader.read(&ik, sizeof(PMDIK) - sizeof(PMDIK::chain))) {
            return false;
        }

        // 可変部分
        if(!binary_reader.read_to_vec(ik.chain, ik.chain_length)) {
            return false;
        }
    }

    return true;
}

bool PMDModelLoader::load_pmd_morph(BinaryReader& binary_reader) {
    uint16_t morph_count{};
    binary_reader.read_to(&morph_count);

    this->morphs.resize(morph_count);
    for(int i = 0; i < morph_count; i++) {
        // 可変部分であるverticesの部分だけは後で読み込むように
        // 先頭25Byteだけ読み込み
        auto& morph = this->morphs[i];
        if(!binary_reader.read(&morph, sizeof(PMDMorph) - sizeof(PMDMorph::vertices))) {
            return false;
        }

        // 可変部分
        if(!binary_reader.read_to_vec(morph.vertices, morph.vertex_count)) {
            return false;
        }
    }

    return true;
}

const std::vector<PMDVertex>& PMDModelLoader::get_vertices(void) const noexcept {
    return this->vertices;
}

const std::vector<BoneIndex>& PMDModelLoader::get_indices(void) const noexcept {
    return this->indices;
}

const std::vector<PMDMaterial>& PMDModelLoader::get_materials(void) const noexcept {
    return this->materials;
}

const std::vector<PMDMorph>& PMDModelLoader::get_morphs(void) const noexcept {
    return this->morphs;
}

const std::vector<PMDBone>& PMDModelLoader::get_bones(void) const noexcept {
    return this->bones;
}

const std::vector<PMDIK>& PMDModelLoader::get_iks(void) const noexcept {
    return this->iks;
}