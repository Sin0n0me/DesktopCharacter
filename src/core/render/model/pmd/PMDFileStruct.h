#pragma once
#include <array>
#include <vector>

#pragma pack(push, 1)

struct PMDHeader {
    char magic[3];	        // "Pmd"
    float version;			// 1.0
    int8_t model_name[20];	//
    int8_t comment[256];	// コメント
};

struct PMDVertex {
    float position[3];		// 座標
    float normal[3];		// 法線
    float uv[2];			// UV
    uint16_t bone_index[2];	// ボーン番号
    uint8_t bone_weight;    // 0-100
    uint8_t edge_flag;		// 輪郭エッジフラグ
};

struct PMDVertexIndex {
    uint16_t index; // ボーン番号
};

struct PMDMaterial {
    float diffuse[4];		// ディフューズカラー
    float specular[3];		// スペキュラ係数
    float shininess;		// スペキュラカラー RGB
    float ambient[3];		// アンビエントカラー RGB
    uint8_t toon_index;		// トゥーンレンダリング用のテクスチャのインデックス
    uint8_t edge_flag;		// 輪郭エッジフラグ
    uint32_t index_count;	// このマテリアルの頂点インデックス数
    char texture_file[20];	// "xxx.png" or "xxx.png*sph"
};

struct PMDBone {
    char name[20];          // ボーン名
    uint16_t parent_index;  // 親ボーン番号
    uint16_t tail_index;    // 表示先ボーン
    uint8_t type;           // ボーンタイプ
    uint16_t ik_parent;     // IK親
    float position[3];      // ボーン位置(モデル空間)
};

// 可変要素を含む
struct PMDIK {
    uint16_t ik_bone;			 // IKボーン番号(足IK)
    uint16_t target_bone;		 // ターゲット(足首)
    uint8_t chain_length;		 // 影響ボーン数
    uint16_t iterations;		 // 反復回数
    float limit;				 // 回転制限(ラジアン)
    std::vector<uint16_t> chain; // IK影響下(リンク)ボーンの番号配列(可変)
};

struct PMDMorphVertex {
    uint32_t index;    // インデックス
    float position[3]; // 座標
};

// 可変要素を含む
struct PMDMorph {
    char name[20];			              // 表情名
    uint32_t vertex_count;	              // 表情用頂点の数
    uint8_t skin_type;		              // 表情の種類
    std::vector<PMDMorphVertex> vertices; // 頂点情報
};

struct PMDDisplayMorph {
    uint16_t index;// 表情枠の表情インデックス
};

struct PMDBoneFrameName {
    std::array<char, 50> frame_name;
};

struct PMDDisplayBone {
    uint16_t bone_index; // 枠用ボーン番号
    uint8_t frame_index; // 表示枠番号
};

struct PMDEnglishDictionary {
    uint8_t is_support;
    char model_name[20];                            // モデル名
    char comment[256];                              // コメント
    std::vector<std::array<char, 20>> bone_name;    // ボーン名
    std::vector<std::array<char, 20>> skin_name;    // スキン名
    std::vector<std::array<char, 50>> display_name; // 表示名
};

struct PMDToonTexture {
    char file_names[10][100];
};

struct PMDRigidBody {
    char name[20];              // 剛体の名前
    uint16_t relate_bone_index; // 関連ボーン番号
    uint8_t group_index;        // 剛体グループ番号
    uint16_t group_target;      // 対象剛体グループマスク
    uint8_t  shape_type;        // 衝突形状( 0:球  1:箱  2:カプセル )
    float shape_size[3];        // 衝突形状の 幅 高さ 奥行( 0:球[0]  1:箱[0][1][2]  2:カプセル[0][1] )
    float position[3];          // 位置(相対座標のオフセット)
    float rotation[3];          // 回転(Radian)
    float mass;                 // 質量
    float linear_damping;       // 移動減
    float angular_damping;      // 回転減
    float restitution;          // 反発力
    float friction;             // 摩擦力
    uint8_t rigid_body_type;    // 剛体タイプ( 0:Bone追従 kinematic  1:物理演算  2:物理演算(Bone位置合わせ) )
};

struct PMDPhysicsJoint {
    char name[20];                   // 名前
    uint32_t rigid_body_a;           // 接続先剛体Ａ
    uint32_t rigid_body_b;           // 接続先剛体Ｂ
    float position[3];               // 位置
    float rotation[3];               // 回転(Radian)
    float constrain_position_min[3]; // 移動制限-下限
    float constrain_position_max[3]; // 移動制限-上限
    float constrain_rotation_min[3]; // 回転制限-下限
    float constrain_rotation_max[3]; // 回転制限-上限
    float spring_position[3];        // ばね移動値
    float spring_rotation[3];        // ばね回転値
};

#pragma pack(pop)

// 以下templateで纏めるべきか...?

struct PMDVertices {
    uint32_t size;
    std::vector<PMDVertex> vertices;
};

struct PMDIndices {
    uint32_t size;
    std::vector<PMDVertexIndex> indices;
};

struct PMDMaterials {
    uint32_t size;
    std::vector<PMDMaterial> materials;
};

struct PMDBones {
    uint16_t size;
    std::vector<PMDBone> bones;
};

struct PMDIKs {
    uint16_t size;
    std::vector<PMDIK> iks;
};

struct PMDMorphs {
    uint16_t size;
    std::vector<PMDMorph> morphs;
};

struct PMDDisplayMorphs {
    uint8_t size;
    std::vector<PMDDisplayMorph> indices;
};

struct PMDBoneFrameNames {
    uint8_t size;
    std::vector<PMDBoneFrameName> frame_names;
};

struct PMDDisplayBones {
    uint32_t size;
    std::vector<PMDDisplayBone> display_bones;
};

struct PMDRigidBodies {
    uint32_t size;
    std::vector<PMDRigidBody> rigid_bodies;
};

struct PMDPhysicsJoints {
    uint32_t size;
    std::vector<PMDPhysicsJoint> physics_joints;
};