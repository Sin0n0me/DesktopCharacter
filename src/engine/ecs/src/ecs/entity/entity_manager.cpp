#pragma once
#include "entity_manager.h"
#include <vector>

namespace enishi::ecs {
    EntityID EntityManager::create(void) {
        if (!this->free_list.empty()) {
            // 削除済みIDを再利用
            EntityID id = this->free_list.back();
            this->free_list.pop_back();
            this->alive[id] = true;
            return id;
        }

        this->next_id += 1;
        EntityID id = this->next_id;
        this->alive.push_back(true);
        return id;
    }

    void EntityManager::destroy(const EntityID id) {
        // assert(isAlive(id));
        this->alive[id] = false;
        this->free_list.push_back(id);
    }

    bool EntityManager::is_alive(const EntityID id) const noexcept {
        return id < this->alive.size() && this->alive[id];
    }
} // namespace enishi::ecs