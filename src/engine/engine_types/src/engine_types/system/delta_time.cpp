#include "delta_time.h"
namespace enishi::types {
    DeltaTime::DeltaTime(const std::chrono::nanoseconds& nano)
        : delta_time(std::chrono::duration_cast<DeltaTime::Resolution>(nano)) {
    }
    DeltaTime::DeltaTime(const std::chrono::microseconds& micro)
        : delta_time(std::chrono::duration_cast<DeltaTime::Resolution>(micro)) {
    }
    DeltaTime::DeltaTime(const std::chrono::milliseconds& mill)
        : delta_time(std::chrono::duration_cast<DeltaTime::Resolution>(mill)) {
    }
    DeltaTime::DeltaTime(const std::chrono::seconds& second)
        : delta_time(std::chrono::duration_cast<DeltaTime::Resolution>(second)) {
    }
    DeltaTime::DeltaTime(const float dt)
        : delta_time(
              std::chrono::duration_cast<DeltaTime::Resolution>(std::chrono::duration<float>(dt))) {
    }
    DeltaTime::DeltaTime(const double dt)
        : delta_time(std::chrono::duration_cast<DeltaTime::Resolution>(
              std::chrono::duration<double>(dt))) {
    }
    float DeltaTime::to_float(void) const {
        return static_cast<float>(this->delta_time.count());
    }
    double DeltaTime::to_double(void) const {
        return static_cast<double>(this->delta_time.count());
    }
    std::int64_t DeltaTime::to_int64(void) const {
        return this->delta_time.count();
    }
    float DeltaTime::to_float_second(void) const {
        return this->to_float() / static_cast<float>(DeltaTime::UNITS_PER_SECOND);
    }
    double DeltaTime::to_double_second(void) const {
        return this->to_double() / static_cast<double>(DeltaTime::UNITS_PER_SECOND);
    }
    std::int64_t DeltaTime::to_nano_second(void) const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(this->delta_time).count();
    }
    std::int64_t DeltaTime::to_micro_second(void) const {
        return std::chrono::duration_cast<std::chrono::microseconds>(this->delta_time).count();
    }
    std::int64_t DeltaTime::to_milli_second(void) const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(this->delta_time).count();
    }
    std::int64_t DeltaTime::to_second(void) const {
        return std::chrono::duration_cast<std::chrono::seconds>(this->delta_time).count();
    }
} // namespace enishi::types