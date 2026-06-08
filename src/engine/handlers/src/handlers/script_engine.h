#pragma once
#include <expected>
#include <string>

namespace enishi::handler {
    class IScriptEngne {
      public:
        [[nodiscard]] virtual std::expected<void, int> compile(void) = 0;

        virtual void run_script(void) = 0;
    };
} // namespace enishi::handler