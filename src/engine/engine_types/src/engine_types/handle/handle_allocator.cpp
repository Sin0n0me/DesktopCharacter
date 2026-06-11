#include "handle_allocator.h"

namespace enishi::types {
    HandleId HandleAllocator::create(void) {
        if (!this->free_list.empty()) {
            // 削除済みIDを再利用
            HandleId id = this->free_list.back();
            this->free_list.pop_back();
            this->alive[id] = true;
            return id;
        }

        this->next_id += 1;
        HandleId id = this->next_id;
        this->alive.push_back(true);

        return id;
    }

    void HandleAllocator::destroy(const HandleId id) {
        if (id < this->alive.size()) {
            this->alive[id] = false;
            this->free_list.push_back(id);
        }
    }

    bool HandleAllocator::is_alive(const HandleId id) const noexcept {
        return id < this->alive.size() && this->alive[id];
    }
} // namespace enishi::types