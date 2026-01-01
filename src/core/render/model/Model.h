#pragma once
#include "../../collider/IModelOBB.h"
#include "../../render/model/IModelRenderer.h"
#include "../../render/shader/IShaderBlueprint.h"
#include "../../render/motion/IMotion.h"
#include <filesystem>

class Model :
	public IModelRenderer,
	public IModelOBB,
	public IShaderBlueprint,
	public IMotion {
public:
	const std::filesystem::path path;

	Model(const std::filesystem::path& path) : path(path) {}

	virtual ~Model(void) = default;

	virtual void on_clicked(const short obb_index) = 0;

	const std::u8string get_file_name(void) const;
};
