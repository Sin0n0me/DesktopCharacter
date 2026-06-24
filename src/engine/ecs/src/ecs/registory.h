#pragma once
#include "component/component_pool.h"
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
    namespace {
        using ComponentId = std::size_t;

        ComponentId next_component_id(void) {
            static ComponentId id = 0;
            return id++;
        }

        template <typename T> ComponentId get_component_id(void) {
            static const ComponentId id = next_component_id();
            return id;
        }
    } // namespace

    class Registory {
      private:
        EntityManager entity_manager;
        std::unordered_map<ComponentId, std::unique_ptr<IComponentPool>> pools;

      public:
        EntityID create(void);

        void destroy(const EntityID id);

        bool is_alive(const EntityID id) const noexcept;

        template <typename T, typename... Args>
        foundation::Result<T&, ECSError> emplace(const EntityID id, Args&&... args) {
            return this->get_pool<T>().emplace(id, std::forward<Args>(args)...);
        }

        template <typename T>
        foundation::Result<T&, ECSError> insert(const EntityID id, T& component) {
            return this->get_pool<T>().insert(id, component);
        }

        template <typename T>
        foundation::Result<T&, ECSError> insert(const EntityID id, T&& component) {
            return this->get_pool<T>().insert(id, component);
        }

        template <typename T>
        foundation::Result<T&, ECSError> insert_or_replace(const EntityID id, T& component) {
            return this->get_pool<T>().insert_or_replace(id, std::move(component));
        }

        template <typename T>
        foundation::Result<T&, ECSError> insert_or_ignore(const EntityID id, T& component) {
            return this->get_pool<T>().insert_or_ignore(id, std::move(component));
        }

        template <typename T> void remove(const EntityID id) {
            this->get_pool<T>().remove(id);
        }

        template <typename T> bool has(const EntityID id) const {
            const auto key = get_component_id<T>();
            auto it = this->pools.find(key);
            return it != this->pools.end() && it->second->has(id);
        }

        template <typename T> foundation::Option<T&> get(const EntityID id) {
            return this->get_pool<T>().get(id);
        }

        template <typename T> foundation::Option<const T&> get(const EntityID id) const {
            return this->get_pool<T>().get(id);
        }

        template <typename... Ts> View<Ts...> view(void) {
            return View<Ts...>(this->get_pool<Ts>()...);
        }

      private:
        template <typename T> ComponentPool<T>& get_pool(void) {
            const auto key = get_component_id<T>();
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
