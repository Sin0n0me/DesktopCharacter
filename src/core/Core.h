#pragma once
#include <memory>
#include <optional>
#include <vector>
#include "D3D11.h"
#include "model/Models.h"
#include "scene/Scene.h"
#include "render_pass/CommonResource.h"
#include "render_pass/IRenderPass.h"

#ifdef _DEBUG
constexpr bool IS_DEBUG_MODE = true;
#else
constexpr bool IS_DEBUG_MODE = false;
#endif // _DEBUG

constexpr float CLEAR_COLOR[4] = {0.0f, 0.0f, 0.0f, 0.0f};

class Engine {
private:
	std::unique_ptr<D3D11> d3d11;
	std::unique_ptr<Models> models;
	std::unique_ptr<Scene> scene;
	std::shared_ptr<CommonResource> common_resouce;
	std::vector<std::unique_ptr<IRenderPass>> render_pass;

	Engine(void) = default;

	void render_update(void);
	void render(void) const;

public:

	static std::optional<Engine> make_engine(const HWND hwnd, const UINT width, const  UINT height);

	int run_app(void);
};
