#pragma once
#include "../../collider/IModelOBB.h"
#include "../../render/model/IModelRenderer.h"
#include "../../render/motion/IMotion.h"
#include <filesystem>
#include <wrl/client.h>

struct ID3D11Buffer;

class Model :
	public IModelRenderer,
	public IModelOBB,
	public IMotion {
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

public:
	const std::filesystem::path path;

	explicit Model(const std::filesystem::path& path);

	virtual ~Model(void) = default;

	virtual void on_clicked(const short obb_index) = 0;

	const std::u8string get_file_name(void) const;
};
