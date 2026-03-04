#include "RenderPass.h"

RenderPass::RenderPass(const std::shared_ptr<CommonResource>& common_resource) {
    this->resource = common_resource;
    this->binding_slots = std::make_unique<ShaderBindingSlots>();
}

const ShaderBindingSlots* RenderPass::get_shader_binding_slots(void) const {
    return this->binding_slots.get();
}