#pragma once
#include "entity.h"
#include <vector>

namespace enishi::ecs {
    class EntityManager {
      private:
        EntityID next_id = 1;
        std::vector<bool> alive;
        std::vector<EntityID> free_list;

      public:
        EntityID create(void);

        void destroy(const EntityID id);

        bool is_alive(const EntityID id) const noexcept;
    };
} // namespace enishi::ecs
