#pragma once
#include <cstdint>

#pragma pack(push, 1)

// 参考
// https://w.atwiki.jp/kumiho_k/pages/15.html

struct VMDHeader {
	char header[30];	 // ファイルシグネチャ30Byte
	char model_name[20]; // モデル名
};

struct VMDBoneKeyFrame {
	char bone_name[15];			//
	uint32_t frame_index;		//
	float translation[3];		//
	float rotation[4];			//
	uint8_t interpolation[64];	//
};

struct VMDMorphKeyframe {
	char morph_name[15];			//
	uint32_t frame_index;
	float weight;
};

struct VMDCameraKeyframe {
	uint32_t frame_index;
	float distance;
	float position[3];
	float rotation[3];
	uint8_t interpolation[24];
	uint32_t view_angle;
	uint8_t perspective;
};

#pragma pack(pop)
