#pragma once
#include <foundation/str/str.h>
#include <memory>
#include <platform/asset/>
#include <platform/renderer/interface_renderer.h>
#include <span>
#include <vector>

namespace enishi::model_controller {
    class ModelContoller {
      private:
      public:
        void change_model(void);

        [[nodiscard]] std::vector<foundation::UTF8> model_list(void);

        void action();
    };
} // namespace enishi::model_controller