#pragma once
#include "component_pool.h"
#include "entity/entity.h"
#include "entity/entity_manager.h"
#include "view.h"
#include <cassert>
#include <cstdint>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace enishi::ecs {
    class Registory {
      private:
        EntityManager entity_manager;
        std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> pools;

      public:
        EntityID create(void);

        void destroy(const EntityID id);

        bool is_alive(const EntityID id) const noexcept;

        template <typename T, typename... Args> T& emplace(const EntityID id, Args&&... args) {
            return this->get_pool<T>().emplace(id, std::forward<Args>(args)...);
        }

        template <typename T> void remove(const EntityID id) {
            this->get_pool<T>().remove(id);
        }

        template <typename T> bool has(const EntityID id) const {
            auto it = this->pools.find(std::type_index(typeid(T)));
            return it != this->pools.end() && it->second->has(id);
        }

        template <typename T> T& get(const EntityID id) {
            return this->get_pool<T>().get(id);
        }

        template <typename... Ts> View<Ts...> view() {
            return View<Ts...>(this->get_pool<Ts>()...);
        }

      private:
        template <typename T> ComponentPool<T>& get_pool() {
            const auto key = std::type_index(typeid(T));
            auto it = this->pools.find(key);
            if (it == this->pools.end()) {
                const auto [ins, _] =
                    this->pools.emplace(key, std::make_unique<ComponentPool<T>>());
                it = ins;
            }
            return static_cast<ComponentPool<T>&>(*it->second);
        }
    };
} // namespace enishi::ecs
