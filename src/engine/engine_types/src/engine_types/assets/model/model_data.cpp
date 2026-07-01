#include "model_data.h"

namespace enishi::types {
    MeshData ModelData::to_mesh_data(void) const {
        if (auto indices = std::get_if<OwnedRenderData<std::uint16_t>>(&this->indices)) {
            return MeshData{
                .vertices = this->vertices.get_render_data(),
                .indices = indices->get_render_data(),
            };
        }

        return MeshData{
            .vertices = this->vertices.get_render_data(),
        };
    }
} // namespace enishi::types