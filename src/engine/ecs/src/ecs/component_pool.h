#pragma once
#include "entity/entity.h"
#include "errors/errors.h"
#include <expected>
#include <vector>

namespace enishi::ecs {
    // 型消去のための基底クラス（Registryが型を知らずに持つため）
    class IComponentPool {
      public:
        virtual ~IComponentPool(void) noexcept = default;
        virtual void remove(EntityID id) = 0;
        virtual bool has(EntityID id) const noexcept = 0;
    };

    template <typename T> class ComponentPool final : public IComponentPool {
      private:
        static constexpr uint32_t INVALID = UINT32_MAX;

        std::vector<uint32_t> sparse; // EntityID → dense添字
        std::vector<EntityID> dense;  // 有効なEntityIDの列
        std::vector<T> components;    // denseと同じ並びのコンポーネント列

      public:
        // コンポーネントを追加
        template <typename... Args> ECSReuslt<T&> emplace(const EntityID id, Args&&... args) {
            if (this->has(id)) {
                return foundation::Error(ECSError::AlreadyHasComponent, u8"already has component");
            }

            // sparse を id の大きさに合わせて拡張
            if (id >= this->sparse.size()) {
                this->sparse.resize(id + 1, ComponentPool::INVALID);
            }

            this->sparse[id] = static_cast<uint32_t>(this->dense.size());
            this->dense.push_back(id);
            this->components.emplace_back(std::forward<Args>(args)...);

            return this->components.back();
        }

        // コンポーネントを削除（swap-and-pop で穴を作らない）
        void remove(const EntityID id) override {
            if (this->has(id)) {
                return;
            }

            uint32_t idx = this->sparse[id];
            EntityID lastId = this->dense.back();

            // 末尾要素を削除位置に移動
            this->components[idx] = std::move(this->components.back());
            this->dense[idx] = lastId;
            this->sparse[lastId] = idx;

            // 末尾を削除
            this->components.pop_back();
            this->dense.pop_back();
            this->sparse[id] = ComponentPool::INVALID;
        }

        bool has(const EntityID id) const noexcept override {
            return id < this->sparse.size() && this->sparse[id] != ComponentPool::INVALID;
        }

        T& get(const EntityID id) {
            assert(has(id));
            return this->components[this->sparse[id]];
        }

        const T& get(const EntityID id) const {
            assert(has(id));
            return this->components[this->sparse[id]];
        }

        // イテレーション用
        std::vector<T>& get_components(void) noexcept {
            return this->components;
        }

        std::vector<EntityID>& entities(void) noexcept {
            return this->dense;
        }
    };
} // namespace enishi::ecs
