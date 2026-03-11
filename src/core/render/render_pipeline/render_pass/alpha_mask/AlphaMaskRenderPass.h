#pragma once
#include "../RenderPass.h"

struct ID3D11Texture2D;
struct ID3D11UnorderedAccessView;
struct ID3D11ShaderResourceView;

class AlphaMaskRenderPass : public RenderPass {
private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resouce_view;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> output_texture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> read_back_texture;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> unordered_access_view;
    mutable std::vector<std::vector<std::uint8_t>> screen_alpha_buffers; // ComputeShaderの結果を受け取る関係上mutable指定(TODO: mutableを使用しない設計)
    uint64_t buffer_index;

public:
    explicit AlphaMaskRenderPass(const std::shared_ptr<CommonResource>& common_resource) noexcept;

    bool init(ID3D11Device* const device, ID3D11RenderTargetView* const render_target_view) override;
    void update(ID3D11DeviceContext* const context) override;
    void render_set(ID3D11DeviceContext* const context, ID3D11RenderTargetView* const render_target_view) const override;
    void render(ID3D11DeviceContext* const context) const override;
    bool should_reset_state(void) const override;
    bool is_render_model(void) const override;
    bool is_post_render(void) const override;
    void back_buffer_resouce(ID3D11DeviceContext* const context, ID3D11ShaderResourceView* const shader_resouce_view) const override;
    RasterizerKind rasterizer_kind(void) const override;

private:

    bool make_buffers(ID3D11Device* const device);
    bool make_uav_buffer(ID3D11Device* const device);
    bool make_read_back_texture(ID3D11Device* const device);
    bool make_constant_buffer(ID3D11Device* const device);
    bool make_shader(ID3D11Device* const device);
    bool make_shader_resouce_view(
        ID3D11Device* const device,
        ID3D11RenderTargetView* const render_target_view
    );

    uint64_t get_next_buffer_index(void) const noexcept;
};
