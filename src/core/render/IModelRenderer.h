#pragma once

struct ID3D11DeviceContext;
struct ID3D11Device;

namespace std::filesystem {
	class path;
}

class IModelRenderer {
public:
	virtual ~IModelRenderer() = default;

	virtual bool init(void) = 0;

	virtual bool load_model(const std::filesystem::path& path, ID3D11Device* const device) = 0;

	virtual void unload_model(void) = 0;

	virtual bool is_loaded_model(void) = 0;

	virtual void update(ID3D11DeviceContext* const context) = 0;

	virtual void render(ID3D11DeviceContext* const context) const = 0;
};
