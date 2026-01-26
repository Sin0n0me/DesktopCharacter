#include "../../../log/Logger.h"
#include "../../../utility/BinaryReader.h"
#include "../../../utility/Maker.h"
#include "PMDModelLoader.h"
#include <optional>

PMDModelLoader::PMDModelLoader(const std::filesystem::path& path) noexcept : path(path) {
    Maker::make_shared(this->vertices);
    Maker::make_shared(this->indices);
    Maker::make_shared(this->materials);
    Maker::make_shared(this->bones);
    Maker::make_shared(this->iks);
    Maker::make_shared(this->morphs);
    Maker::make_shared(this->display_morphs);
    Maker::make_shared(this->bone_frame_name);
    Maker::make_shared(this->display_bones);
    Maker::make_shared(this->english_dictionary);
    Maker::make_shared(this->toon_textures);
    Maker::make_shared(this->rigid_bodies);
    Maker::make_shared(this->physics_joints);
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
    if(!this->load_pmd_morphs(binary_reader)) {
        return false;
    }

    // モーフの表示リスト
    if(!this->load_pmd_display_morphs(binary_reader)) {
        return false;
    }

    // ボーンフレーム
    if(!this->load_pmd_bone_frame_names(binary_reader)) {
        return false;
    }

    // 表示ボーン
    if(!this->load_pmd_display_bones(binary_reader)) {
        return false;
    }

    // 以下拡張部分

    // 英名対応
    if(!this->load_pmd_english_dictionary(binary_reader)) {
        return false;
    }

    // トゥーンテクスチャリスト
    if(!this->load_pmd_toon_textures(binary_reader)) {
        return false;
    }

    // 物理剛体
    if(!this->load_pmd_rigid_bodies(binary_reader)) {
        return false;
    }

    // ジョイント
    if(!this->load_pmd_physics_joints(binary_reader)) {
        return false;
    }

    if(!binary_reader.is_eof()) {
        Logger::warning(
            u8"EOFに達しませんでした. ファイルが破損しているか未対応の拡張データがあります"
        );
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
    if(!binary_reader.read_to(&this->vertices->size)) {
        Logger::error(u8"頂点情報リストサイズの取得に失敗しました");
        return false;
    }

    if(!binary_reader.read_to_vec(
        this->vertices->vertices,
        this->vertices->size
    )) {
        Logger::error(u8"頂点情報の取得に失敗しました");
        return false;
    }

    Logger::info(u8"頂点情報の読み込みに成功しました");
    return true;
}

bool PMDModelLoader::load_pmd_indices(BinaryReader& binary_reader) {
    if(!binary_reader.read_to(&this->indices->size)) {
        Logger::error(u8"インデックスリストサイズの取得に失敗しました");
        return false;
    }

    if(!binary_reader.read_to_vec(
        this->indices->indices,
        this->indices->size
    )) {
        Logger::error(u8"インデックス情報の取得に失敗しました");
        return false;
    }

    Logger::info(u8"インデックス情報の読み込みに成功しました");
    return true;
}

bool PMDModelLoader::load_pmd_materials(BinaryReader& binary_reader) {
    if(!binary_reader.read_to(&this->materials->size)) {
        Logger::error(u8"マテリアルリストサイズの取得に失敗しました");
        return false;
    }

    if(!binary_reader.read_to_vec(
        this->materials->materials,
        this->materials->size
    )) {
        Logger::error(u8"マテリアル情報の取得に失敗しました");
        return false;
    }

    Logger::info(u8"マテリアル情報の読み込みに成功しました");
    return true;
}

bool PMDModelLoader::load_pmd_bones(BinaryReader& binary_reader) {
    if(!binary_reader.read_to(&this->bones->size)) {
        Logger::error(u8"ボーンリストサイズの取得に失敗しました");
        return false;
    }

    if(!binary_reader.read_to_vec(
        this->bones->bones,
        this->bones->size
    )) {
        Logger::error(u8"ボーン情報の取得に失敗しました");
        return false;
    }

    Logger::info(u8"ボーン情報の読み込みに成功しました");
    return true;
}

bool PMDModelLoader::load_pmd_iks(BinaryReader& binary_reader) {
    if(!binary_reader.read_to(&this->iks->size)) {
        Logger::error(u8"IKリストサイズの取得に失敗しました");
        return false;
    }

    this->iks->iks.resize(this->iks->size);
    for(auto& ik : this->iks->iks) {
        // 可変部分であるchainの部分だけは後で読み込むように
        // 先頭11Byteだけ読み込み
        if(!binary_reader.read(
            &ik,
            sizeof(PMDIK) - sizeof(PMDIK::chain)
        )) {
            Logger::error(u8"IK情報の取得に失敗しました");
            return false;
        }

        // 可変部分
        if(!binary_reader.read_to_vec(
            ik.chain,
            ik.chain_length
        )) {
            Logger::error(u8"IKのリンク情報の取得に失敗しました");
            return false;
        }
    }

    Logger::info(u8"IK情報の読み込みに成功しました");
    return true;
}

bool PMDModelLoader::load_pmd_morphs(BinaryReader& binary_reader) {
    if(!binary_reader.read_to(&this->morphs->size)) {
        Logger::error(u8"モーフリストサイズの取得に失敗しました");
        return false;
    }

    this->morphs->morphs.resize(this->morphs->size);
    for(auto& morph : this->morphs->morphs) {
        // 可変部分であるverticesの部分だけは後で読み込むように
        // 先頭25Byteだけ読み込み
        if(!binary_reader.read(
            &morph,
            sizeof(PMDMorph) - sizeof(PMDMorph::vertices)
        )) {
            Logger::error(u8"モーフ情報の取得に失敗しました");
            return false;
        }

        // 可変部分
        if(!binary_reader.read_to_vec(
            morph.vertices,
            morph.vertex_count
        )) {
            Logger::error(u8"モーフの頂点情報の取得に失敗しました");
            return false;
        }
    }

    Logger::info(u8"モーフ情報の読み込みに成功しました");
    return true;
}

bool PMDModelLoader::load_pmd_display_morphs(BinaryReader& binary_reader) {
    if(!binary_reader.read_to(&this->display_morphs->size)) {
        Logger::error(u8"モーフの表示リストのサイズ取得に失敗しました");
        return false;
    }

    if(!binary_reader.read_to_vec(
        this->display_morphs->indices,
        this->display_morphs->size
    )) {
        Logger::error(u8"モーフの表示リスト取得に失敗しました");
        return false;
    }

    Logger::info(u8"モーフ表示情報の読み込みに成功しました");
    return true;
}

bool PMDModelLoader::load_pmd_bone_frame_names(BinaryReader& binary_reader) {
    if(!binary_reader.read_to(&this->bone_frame_name->size)) {
        Logger::error(u8"ボーン枠名リストのサイズ取得に失敗しました");
        return false;
    }

    if(!binary_reader.read_to_vec(
        this->bone_frame_name->frame_names,
        this->bone_frame_name->size
    )) {
        Logger::error(u8"ボーン枠名の取得に失敗しました");
        return false;
    }

    Logger::info(u8"ボーン枠名情報の読み込みに成功しました");
    return true;
}

bool PMDModelLoader::load_pmd_display_bones(BinaryReader& binary_reader) {
    if(!binary_reader.read_to(&this->display_bones->size)) {
        Logger::error(u8"ボーン表示名リストの取得に失敗しました");
        return false;
    }

    if(!binary_reader.read_to_vec(
        this->display_bones->display_bones,
        this->display_bones->size
    )) {
        Logger::error(u8"ボーン表示名の取得に失敗しました");
        return false;
    }

    Logger::info(u8"ボーン表示名情報の読み込みに成功しました");
    return true;
}

bool PMDModelLoader::load_pmd_english_dictionary(BinaryReader& binary_reader) {
    if(!binary_reader.read_to(&this->english_dictionary->is_support)) {
        Logger::error(u8"英名対応フラグの取得に失敗しました");
        return false;
    }

    // 英語非対応の場合はそのまま次へ
    if(this->english_dictionary->is_support == 0) {
        Logger::info(u8"このモデルは英名対応していません");
        return true;
    }

    // 順に読み込む
    if(!binary_reader.read_to(&this->english_dictionary->model_name)) {
        Logger::error(u8"モデル名(英名)取得に失敗しました");
        return false;
    }
    if(!binary_reader.read_to(&this->english_dictionary->comment)) {
        Logger::error(u8"コメント(英名)取得に失敗しました");
        return false;
    }

    // 可変部分
    if(!binary_reader.read_to_vec(
        this->english_dictionary->bone_name,
        this->bones->size
    )) {
        Logger::error(u8"ボーン名(英名)取得に失敗しました");
        return false;
    }

    if(!binary_reader.read_to_vec(
        this->english_dictionary->skin_name,
        this->morphs->size - 1
    )) {
        Logger::error(u8"スキン名(英名)取得に失敗しました");
        return false;
    }

    if(!binary_reader.read_to_vec(
        this->english_dictionary->display_name,
        this->bone_frame_name->size
    )) {
        Logger::error(u8"ボーン枠名(英名)取得に失敗しました");
        return false;
    }

    Logger::info(u8"英名対応辞書の読み込みに成功しました");
    return true;
}

bool PMDModelLoader::load_pmd_toon_textures(BinaryReader& binary_reader) {
    if(!binary_reader.read_to(&this->toon_textures->file_names)) {
        Logger::error(u8"トゥーンテクスチャの取得に失敗しました");
        return false;
    }

    Logger::info(u8"トゥーンテクスチャの読み込みに成功しました");
    return true;
}

bool PMDModelLoader::load_pmd_rigid_bodies(BinaryReader& binary_reader) {
    if(!binary_reader.read_to(&this->rigid_bodies->size)) {
        Logger::error(u8"剛体情報リストのサイズ読み込みに失敗しました");
        return false;
    }

    if(!binary_reader.read_to_vec(
        this->rigid_bodies->rigid_bodies,
        this->rigid_bodies->size
    )) {
        Logger::error(u8"剛体情報の読み込みに失敗しました");
        return false;
    }

    Logger::info(u8"剛体情報の読み込みに成功しました");
    return true;
}

bool PMDModelLoader::load_pmd_physics_joints(BinaryReader& binary_reader) {
    if(!binary_reader.read_to(&this->physics_joints->size)) {
        Logger::error(u8"物理ジョイントリストのサイズ読み込みに失敗しました");
        return false;
    }

    if(!binary_reader.read_to_vec(
        this->physics_joints->physics_joints,
        this->physics_joints->size
    )) {
        Logger::error(u8"物理ジョイント情報の読み込みに失敗しました");
        return false;
    }

    Logger::info(u8"物理ジョイント情報の読み込みに成功しました");
    return true;
}

std::shared_ptr<const PMDVertices> PMDModelLoader::get_vertices(void) const noexcept {
    return this->vertices;
}

std::shared_ptr<const PMDIndices> PMDModelLoader::get_indices(void) const noexcept {
    return this->indices;
}

std::shared_ptr<const PMDMaterials> PMDModelLoader::get_materials(void) const noexcept {
    return this->materials;
}

std::shared_ptr<const PMDBones> PMDModelLoader::get_bones(void) const noexcept {
    return this->bones;
}

std::shared_ptr<const PMDIKs> PMDModelLoader::get_iks(void) const noexcept {
    return this->iks;
}

std::shared_ptr<const PMDMorphs> PMDModelLoader::get_morphs(void) const noexcept {
    return this->morphs;
}

std::shared_ptr<const PMDDisplayMorphs> PMDModelLoader::get_display_morphs(void) const noexcept {
    return this->display_morphs;
}

std::shared_ptr<const PMDBoneFrameNames> PMDModelLoader::get_bone_frame_name(void) const noexcept {
    return this->bone_frame_name;
}

std::shared_ptr<const PMDDisplayBones> PMDModelLoader::get_display_bones(void) const noexcept {
    return this->display_bones;
}

std::shared_ptr<const PMDEnglishDictionary> PMDModelLoader::get_english_dictionary(void) const noexcept {
    return this->english_dictionary;
}

std::shared_ptr<const PMDToonTexture> PMDModelLoader::get_toon_textures(void) const noexcept {
    return this->toon_textures;
}

std::shared_ptr<const PMDRigidBodies> PMDModelLoader::get_rigid_bodies(void) const noexcept {
    return this->rigid_bodies;
}

std::shared_ptr<const PMDPhysicsJoints> PMDModelLoader::get_physics_joints(void) const noexcept {
    return this->physics_joints;
}