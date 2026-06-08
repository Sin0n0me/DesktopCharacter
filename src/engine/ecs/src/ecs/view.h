#include <tuple>

namespace enishi::ecs {
    template <typename... Ts> class View {
      private:
        std::tuple<ComponentPool<Ts>&...> pools;
        uint32_t current_id = 0;

      private:
        // 全コンポーネントを持つエンティティまでスキップ
        void skip_invalid(const uint32_t start) {
            this->current_id = start;
            auto& entities = this->base_pool().entities();
            while (
                this->current_id < entities.size() && !this->has_all(entities[this->current_id])) {
                ++this->current_id;
            }
        }

        bool has_all(const EntityID id) const {
            return (... && std::get<ComponentPool<Ts>&>(pools).has(id));
        }

        auto& base_pool(void) {
            return std::get<ComponentPool, std::tuple_element_t<0, std::tuple<Ts...>>>(this->pools);
        }

      public:
        explicit View(const ComponentPool<Ts>&... pools)
            : pools(pools...) {
        }

        // range-based for で使えるイテレータ
        struct Iterator {
            View& view;
            uint32_t idx;

            bool operator!=(const Iterator& other) const {
                return this->idx != other.idx;
            }
            Iterator& operator++() {
                ++this->idx;
                return *this;
            }

            // [EntityID, Component&...] のタプルを返す
            auto operator*() {
                // 最もエンティティ数が少ないPoolのdenseを基準にする
                EntityID id = this->base_pool().entities()[this->idx];
                return std::tuple<EntityID, Ts&...>(
                    id, std::get<ComponentPool<Ts>&>(this->view.pools).get(id)...);
            }

            ComponentPool<std::tuple_element_t<0, std::tuple<Ts...>>>& base_pool() {
                return std::get<0>(view.pools);
            }
        };

        Iterator begin(void) {
            this->skip_invalid(0);
            return Iterator{*this, this->current_id};
        }

        Iterator end(void) {
            return Iterator{*this, static_cast<uint32_t>(this->base_pool().entities().size())};
        }
    };
} // namespace enishi::ecs