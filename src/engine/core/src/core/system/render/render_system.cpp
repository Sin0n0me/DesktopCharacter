#include "render_system.h"
#include <component/animation_component.h>
#include <component/model_component.h>

namespace enishi::core {
    RenderSystem::RenderSystem(std::shared_ptr<ecs::Registory> registory)
        : registory(registory) {
    }

    void RenderSystem::update(const types::DeltaTime& delta_time) {
        auto view =
            this->registory->view<component::AnimationComponent, component::ModelComponent>();

        for (auto [entity, animation, model] : view) {
        }
    }

    const types::RenderGraph& core::RenderSystem::get_render_graph(void) const {
        return this->render_graph;
    }

    void core::RenderSystem::add_command() {
    }
} // namespace enishi::core