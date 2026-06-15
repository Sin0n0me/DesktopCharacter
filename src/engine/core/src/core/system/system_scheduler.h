#pragma once
#include "system.h"
#include <memory>
#include <vector>

namespace enishi::core {
    class SystemScheduler {
      private:
        std::vector<std::shared_ptr<ISystem>> system;

      public:
        template <typename T, typename... Args>
        void register_system(const std::uint32_t priority, const Args&... arg) {
            this->system.push_back(std::make_unique<T>(arg...));

            // そもそも登録が頻繁に呼ばれないので毎回ソートでよい
        }
    };
} // namespace enishi::core