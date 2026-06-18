#include "application_timer.h"

namespace enishi::core {
    ApplicationTimer::ApplicationTimer(void)
        : last_time(std::chrono::steady_clock::now())
        , elapsed(0.0f) {
    }

    types::DeltaTime ApplicationTimer::tick(void) noexcept {
        const auto now = std::chrono::steady_clock::now();
        const auto delta = types::DeltaTime(now - this->last_time);
        this->last_time = now;
        this->elapsed += delta.to_float();
        return this->clamp(delta);
    }

    types::DeltaTime ApplicationTimer::clamp(const types::DeltaTime& dt) const noexcept {
        return dt < ApplicationTimer::MAX_DELTA_TIME ? dt : ApplicationTimer::MAX_DELTA_TIME;
    }
} // namespace enishi::core