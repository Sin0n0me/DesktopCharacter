#pragma once
#include <vector>

#pragma pack(push, 1)

struct PMDHeader {
	char magic[3];			// "Pmd"
	float version;			// 1.0
	char modelName[20];		//
	char comment[256];		// コメント
};

struct PMDVertex {
	float position[3];		// 座標
	float normal[3];		// 法線
	float uv[2];			// UV
	uint16_t bone_index[2];	// ボーン番号
	uint8_t bone_weight;    // 0-100
	uint8_t edge_flag;		// 輪郭エッジフラグ
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

// これだけ可変要素を含む
struct PMDIK {
	uint16_t ik_bone;			 // IKボーン番号(足IK)
	uint16_t target_bone;		 // ターゲット(足首)
	uint8_t chain_length;		 // 影響ボーン数
	uint16_t iterations;		 // 反復回数
	float limit;				 // 回転制限(ラジアン)
	std::vector<uint16_t> chain; // IK影響下(リンク)ボーンの番号配列(可変)
};

struct PMDMorph {
	char name[20];			// 表情名
	uint32_t vertex_count;	// 表情用頂点の数
	uint8_t skin_type;		// 表情の種類
};

struct PMDDispBone {
	uint16_t bone_index;           // 枠用ボーン番号
	uint8_t bone_disp_frame_index; // 表示枠番号
};

#pragma pack(pop)