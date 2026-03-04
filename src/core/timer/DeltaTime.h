#pragma once
#include <chrono>

class DeltaTime {
public:
    using Resolution = std::chrono::microseconds;

public:
    static constexpr int64_t UNITS_PER_SECOND = Resolution::period::den;

public:
    const Resolution delta_time;

public:
    explicit DeltaTime(const std::chrono::nanoseconds& nano);
    explicit DeltaTime(const std::chrono::microseconds& micro);
    explicit DeltaTime(const std::chrono::milliseconds& mill);
    explicit DeltaTime(const std::chrono::seconds& second);
    explicit DeltaTime(const float dt);
    explicit DeltaTime(const double dt);

    float to_float(void) const;
    double to_double(void) const;
    int64_t to_int64(void) const;
    float to_float_second(void) const;
    double to_double_second(void) const;

    int64_t to_nano_seconds(void) const;
    int64_t to_micro_seconds(void) const;
    int64_t to_milli_second(void) const;
    int64_t to_second(void) const;
};
