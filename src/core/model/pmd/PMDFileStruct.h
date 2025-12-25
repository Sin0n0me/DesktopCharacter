#pragma once
#include <vector>

#pragma pack(push, 1)

struct PMDHeader {
	char magic[3];      // "Pmd"
	float version;      // 1.0
	char modelName[20];
	char comment[256];
};

struct PMDVertex {
	float pos[3];
	float normal[3];
	float uv[2];
	uint16_t bone_index[2];
	uint8_t bone_weight;   // 0-100
	uint8_t edge_flag;
};

struct PMDMaterial {
	float diffuse[4];
	float specular[3];
	float shininess;
	float ambient[3];
	uint8_t toon_index;
	uint8_t edge_flag;
	uint32_t index_count;  //
	char texture_file[20]; // "xxx.png" or "xxx.png*sph"
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
	std::vector<uint16_t> chain; // チェーン(可変)
};

#pragma pack(pop)