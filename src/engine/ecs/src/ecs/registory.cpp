#include "registory.h"
namespace enishi::ecs {
    EntityID Registory::create(void) {
        return this->entity_manager.create();
    }

    void Registory::destroy(const EntityID id) {
        // 全Poolから該当エンティティのコンポーネントを削除
        for (auto& [type, pool] : this->pools) {
            if (pool->has(id)) {
                pool->remove(id);
            }
        }
        this->entity_manager.destroy(id);
    }

    bool Registory::is_alive(const EntityID id) const noexcept {
        return this->entity_manager.is_alive(id);
    }
} // namespace enishi::ecs