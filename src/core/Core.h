#pragma once
#include "collider/Collider.h"
#include "D3D11.h"
#include "event/mouse/MouseState.h"
#include "IEngineStarter.h"
#include "render/CommonResource.h"
#include "render/model/ModelManager.h"
#include "render/render_pass/IRenderPass.h"
#include "scene/Scene.h"
#include <memory>
#include <optional>
#include <vector>

#ifdef _DEBUG
constexpr bool IS_DEBUG_MODE = true;
#else
constexpr bool IS_DEBUG_MODE = false;
#endif // _DEBUG

constexpr float CLEAR_COLOR[4] = {0.0f, 0.0f, 0.0f, 0.0f};

class Engine : public IEngineStarter {
private:
	friend class WindowEvent; // •ª—£—p

private:
	static std::optional<Engine* const> instance;

private:
	HWINEVENTHOOK hook;
	std::unique_ptr<D3D11> d3d11;
	std::unique_ptr<ModelManager> models;
	std::unique_ptr<Scene> scene;
	std::unique_ptr<MouseState> mouse_state;
	std::unique_ptr<Collider> collider;
	std::shared_ptr<CommonResource> common_resouce;
	std::vector<std::unique_ptr<IRenderPass>> render_pass;

private:
	void update(const int64_t delta_time);
	void render_update(void);
	void render(void) const;

public:

	//static Engine get_engine();

	const IMouseStateGetter* get_mouse_getter(void) const;

	bool init(const HWND& hwnd, const UINT& width, const UINT& height) override;
	void run(void) override;
	void uninit(void) override;
};
