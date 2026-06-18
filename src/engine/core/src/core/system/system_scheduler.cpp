#include "system_scheduler.h"

namespace enishi::core {
    void SystemScheduler::update(const types::DeltaTime& dt) {
        for (auto& entry : this->managed_systems) {
            entry.system->update(dt);
        }
    }
} // namespace enishi::core