#include "view.h"

namespace enishi::renderer::directx {
    foundation::Option<const ViewInfo&> ViewPool::get_view_info(
        const types::HandleId id) const noexcept {
        const auto& iter = this->handle_map.find(id);
        if (iter == this->handle_map.end()) {
            return {};
        }

        return iter->second;
    }

    foundation::VoidResult<DirectXError> ViewPool::create(
        const types::HandleId id, const types::ImageViewType view_type) noexcept {
        if (this->handle_map.contains(id)) {
            return foundation::Error(DirectXError::ViewError, "");
        }

        switch (view_type) {
            case types::ImageViewType::DepthStencil: {
                this->depth_stencils.push_back({});
                this->handle_map.emplace(id,
                    ViewInfo{
                        .view_type = view_type,
                        .index = static_cast<std::uint32_t>(this->depth_stencils.size()) - 1,
                    });
            } break;
            case types::ImageViewType::RenderTarget: {
                this->render_targets.push_back({});
                this->handle_map.emplace(id,
                    ViewInfo{
                        .view_type = view_type,
                        .index = static_cast<std::uint32_t>(this->render_targets.size()) - 1,
                    });
            } break;
            case types::ImageViewType::ShaderResource: {
                this->shader_resources.push_back({});
                this->handle_map.emplace(id,
                    ViewInfo{
                        .view_type = view_type,
                        .index = static_cast<std::uint32_t>(this->shader_resources.size()) - 1,
                    });
            } break;
            case types::ImageViewType::UnorderedAccess: {
                this->unordered_accesses.push_back({});
                this->handle_map.emplace(id,
                    ViewInfo{
                        .view_type = view_type,
                        .index = static_cast<std::uint32_t>(this->unordered_accesses.size()) - 1,
                    });
            } break;
            default:
                return foundation::Error(DirectXError::ViewError, "");
        }

        return {};
    }

    foundation::Option<types::ImageViewType> ViewPool::get_view_type(
        const types::HandleId id) const noexcept {
        const auto opt_info = this->get_view_info(id);
        if (opt_info.is_none()) {
            return {};
        }
        const auto info = opt_info.unwrap();

        return info.view_type;
    }

    foundation::Option<ID3D11DepthStencilView*> ViewPool::get_depth_stencil_view(
        const types::HandleId id) const noexcept {
        const auto opt_info = this->get_view_info(id);
        if (opt_info.is_none()) {
            return {};
        }
        const auto index = opt_info.unwrap().index;

        return this->depth_stencils[index].Get();
    }

    foundation::Option<ID3D11RenderTargetView*> ViewPool::get_render_target_view(
        const types::HandleId id) const noexcept {
        const auto opt_info = this->get_view_info(id);
        if (opt_info.is_none()) {
            return {};
        }
        const auto index = opt_info.unwrap().index;

        return this->render_targets[index].Get();
    }

    foundation::Option<ID3D11ShaderResourceView*> ViewPool::get_shader_resource_view(
        const types::HandleId id) const noexcept {
        const auto opt_info = this->get_view_info(id);
        if (opt_info.is_none()) {
            return {};
        }
        const auto index = opt_info.unwrap().index;

        return this->shader_resources[index].Get();
    }

    foundation::Option<ID3D11UnorderedAccessView*> ViewPool::get_unordered_access_view(
        const types::HandleId id) const noexcept {
        const auto opt_info = this->get_view_info(id);
        if (opt_info.is_none()) {
            return {};
        }
        const auto index = opt_info.unwrap().index;

        return this->unordered_accesses[index].Get();
    }

    foundation::Option<ID3D11DepthStencilView* const*> ViewPool::get_address_depth_stencil_view(
        const types::HandleId id) const noexcept {
        const auto opt_info = this->get_view_info(id);
        if (opt_info.is_none()) {
            return {};
        }
        const auto index = opt_info.unwrap().index;

        return this->depth_stencils[index].GetAddressOf();
    }

    foundation::Option<ID3D11RenderTargetView* const*> ViewPool::get_address_render_target_view(
        const types::HandleId id) const noexcept {
        const auto opt_info = this->get_view_info(id);
        if (opt_info.is_none()) {
            return {};
        }
        const auto index = opt_info.unwrap().index;

        return this->render_targets[index].GetAddressOf();
    }

    foundation::Option<ID3D11ShaderResourceView* const*> ViewPool::get_address_shader_resource_view(
        const types::HandleId id) const noexcept {
        const auto opt_info = this->get_view_info(id);
        if (opt_info.is_none()) {
            return {};
        }
        const auto index = opt_info.unwrap().index;

        return this->shader_resources[index].GetAddressOf();
    }

    foundation::Option<ID3D11UnorderedAccessView* const*>
    ViewPool::get_address_unordered_access_view(const types::HandleId id) const noexcept {
        const auto opt_info = this->get_view_info(id);
        if (opt_info.is_none()) {
            return {};
        }
        const auto index = opt_info.unwrap().index;

        return this->unordered_accesses[index].GetAddressOf();
    }

    const std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>>& ViewPool::get_render_targets(
        void) const noexcept {
        return this->render_targets;
    }
} // namespace enishi::renderer::directx