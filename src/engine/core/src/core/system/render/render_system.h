#pragma once
#include "../interface_system.h"
#include <ecs/registory.h>
#include <engine_types/renderer/render_graph.h>

namespace enishi::core {
    class RenderSystem : public ISystem {
      private:
        std::shared_ptr<ecs::Registory> registory;
        types::RenderGraph render_graph;

      public:
        void update(const types::DeltaTime& delta_time) override;

        const types::RenderGraph& get_render_graph(void) const;

      private:
        void add_command();

        void update_command();
    };
} // namespace enishi::core
