#pragma once
#include "asset_handle.h"
#include "asset_pool.h"
#include "interface_asset_system.h"
#include <filesystem>
#include <foundation/str/str.h>
#include <unordered_map>
#include <unordered_set>

namespace enishi::assets_system {
    namespace {
        using AssetId = std::size_t;

        AssetId next_asset_id(void) {
            static AssetId id = 0;
            return id++;
        }

        template <typename T> AssetId get_asset_id(void) {
            static const AssetId id = next_asset_id();
            return id;
        }
    } // namespace

    class AssetManager {
      private:
        std::unordered_map<std::filesystem::path, AssetHandle> path_to_handle;

      public:
        template <typename T, typename... Args>
        T& emplace(const types::HandleId id, Args&&... args) {
            return this->get_pool<T>().emplace(id, std::forward<Args>(args)...);
        }

        template <typename T> void remove(const types::HandleId id) {
            this->get_pool<T>().remove(id);
        }

        template <typename T> bool has(const types::HandleId id) const {
            const auto key = get_asset_id<T>();
            auto it = this->pools.find(key);
            return it != this->pools.end() && it->second->has(id);
        }

        template <typename T> foundation::Option<T&> get(const types::HandleId id) {
            return this->get_pool<T>().get(id);
        }

      private:
        template <typename T> AssetPool<T>& get_pool(void) {
            const auto key = get_asset_id<T>();
            auto it = this->pools.find(key);
            if (it == this->pools.end()) {
                const auto [ins, _] = this->pools.emplace(key, std::make_unique<AssetPool<T>>());
                it = ins;
            }
            return static_cast<AssetPool<T>&>(*it->second);
        }
    };
} // namespace enishi::assets_system