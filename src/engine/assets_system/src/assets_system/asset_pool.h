#pragma once
#include "asset_handle.h"
#include "errors/errors.h"
#include "interface_asset_system.h"
#include <filesystem>
#include <foundation/str/str.h>
#include <unordered_map>
#include <unordered_set>

namespace enishi::assets_system {
    // 型を知らずにアセットデータを取得できるようにするクラス
    // ECSとほぼ同じ
    class IAssetPool {
      public:
        virtual ~IAssetPool(void) noexcept = default;
        virtual void remove(const types::HandleId id) = 0;
        virtual bool has(const types::HandleId id) const noexcept = 0;
    };

    template <typename T> class AssetPool final : public IAssetPool {
      private:
        static constexpr std::uint32_t INVALID = UINT32_MAX;

        std::vector<std::uint32_t> sparse;  // types::HandleId -> dense添字
        std::vector<types::HandleId> dense; // 有効なtypes::HandleIdの列
        std::vector<T> assets;              // denseと同じ並びのアセット列

      public:
        // アセットを追加
        template <typename... Args>
        foundation::EngineResult<T&, AssetError> emplace(const types::HandleId id, Args&&... args) {
            if (this->has(id)) {
                return foundation::Error(AssetError::AlreadyHasAsset, u8"already has asset");
            }

            // sparse を id の大きさに合わせて拡張
            const auto next_id = id + 1;
            if (this->sparse.size() < next_id) {
                this->sparse.resize(next_id, AssetPool::INVALID);
            }

            this->sparse[id] = static_cast<std::uint32_t>(this->dense.size());
            this->dense.push_back(id);
            this->assets.emplace_back(std::forward<Args>(args)...);

            return this->assets.back();
        }

        // アセットを削除(swap-and-pop で穴を作らない)
        void remove(const types::HandleId id) override {
            if (this->has(id)) {
                return;
            }

            const std::uint32_t idx = this->sparse[id];
            const types::HandleId lastId = this->dense.back();

            // 末尾要素を削除位置に移動
            this->assets[idx] = std::move(this->assets.back());
            this->dense[idx] = lastId;
            this->sparse[lastId] = idx;

            // 末尾を削除
            this->assets.pop_back();
            this->dense.pop_back();
            this->sparse[id] = AssetPool::INVALID;
        }

        bool has(const types::HandleId id) const noexcept override {
            return id < this->sparse.size() && this->sparse[id] != AssetPool::INVALID;
        }

        foundation::Option<T&> get(const types::HandleId id) {
            if (!this->has(id)) {
                return {};
            }
            return this->assets[this->sparse[id]];
        }

        foundation::Option<const T&> get(const types::HandleId id) const {
            if (!this->has(id)) {
                return {};
            }
            return this->assets[this->sparse[id]];
        }

        // イテレーション用
        std::vector<T>& get_components(void) noexcept {
            return this->assets;
        }

        std::vector<types::HandleId>& entities(void) noexcept {
            return this->dense;
        }
    };
} // namespace enishi::assets_system