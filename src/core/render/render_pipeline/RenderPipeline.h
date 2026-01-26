#pragma once
#include "../rasterizer/RasterizerKind.h"
#include "../render_pass/RenderPass.h"
#include "../render_target_view/RenderTargetView.h"
#include "RenderPassName.h"
#include <d3d11.h>
#include <deque>
#include <memory>
#include <unordered_map>

struct D3D11;
class Model;

class RenderPipeline {
private:
    const std::shared_ptr<D3D11> d3d11;

    std::shared_ptr<Model> model;
    std::deque<std::shared_ptr<RenderPass>> pipe_line;
    std::unordered_map<RenderPassName, std::shared_ptr<RenderPass>> render_pass_map;
    std::unordered_map<RenderTargetView, Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> render_target_view;
    std::unordered_map<RasterizerKind, Microsoft::WRL::ComPtr<ID3D11RasterizerState>> rasterizer;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resouce_view; // バックバッファ用

    bool contains(const RenderPassName& name);
    void push_back(const RenderPassName& name);
    void push_front(const RenderPassName& name);
    void insert(const RenderPassName& name, const uint32_t& index);
    void remove(const RenderPassName& name);
    void set(const std::vector<RenderPassName>& names);

    bool make_rasterizer(void);
    bool make_render_target_views(void);
    bool make_offscreen_render_target_view(void);
    bool make_back_buffer_render_target_view(void);

    void reset_state(void) const;

public:
    explicit RenderPipeline(
        const std::shared_ptr<D3D11>& d3d11,
        const std::shared_ptr<CommonResource>& resouce
    ) noexcept;

    bool init(void);

    void render_update(void);

    void render(void) const;

    void on_model_changed(const std::shared_ptr<Model>& model);
};
