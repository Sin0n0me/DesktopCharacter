#pragma once
#include <unordered_map>
#include <string>
#include "Bone.h"

class IBoneNameResolver {
public:
	virtual ~IBoneNameResolver(void) = default;

	virtual bool resolve_bones(const std::unordered_map<std::string, BoneIndex>& bone_name_map) = 0;
};
