#pragma once
#include <algorithm>
#include <cmath>
#include <concepts>

#undef min
#undef max

enum class AngleKind {
    Radian,
    Degree
};

namespace details {
    template <typename T>
    concept Float = std::same_as<T, float>
        || std::same_as<T, double>
        || std::same_as<T, long double>;
}

// 弧度法と度数法が混じり間違えまくって辛かったのでコンパイル時に単位が合わなければエラーになるように自作
template <typename T = float, AngleKind Kind = AngleKind::Radian>
    requires details::Float<T>
class Angle {
private:
    friend class Angle;

public:
    using AngleType = T;
    using Radian = Angle<T, AngleKind::Radian>;
    using Degree = Angle<T, AngleKind::Degree>;

public:
    static constexpr T PI = static_cast<T>(3.14159265358979323846);
    static constexpr AngleKind ANGLE_KIND = Kind;

private:
    T angle;

public:
    constexpr explicit Angle(const T& angle) : angle(angle) {};
    constexpr explicit Angle(const Angle<T, AngleKind::Radian>& angle) :
        angle(angle.to_radian_value()) {
    };
    constexpr explicit Angle(const Angle<T, AngleKind::Degree>& angle) :
        angle(angle.to_degree_value()) {
    };

public:
    constexpr Angle<T, AngleKind::Radian> to_radian(void) const {
        return Angle::Radian(this->to_radian_value());
    }
    constexpr Angle<T, AngleKind::Degree> to_degree(void) const {
        return Angle::Degree(this->to_degree_value());
    }

    constexpr T to_radian_value(void) const {
        if constexpr(Angle::ANGLE_KIND == AngleKind::Degree) {
            return this->angle * (Angle::PI / static_cast<T>(180.0));
        } else {
            return this->angle;
        }
    }
    constexpr T to_degree_value(void) const {
        if constexpr(Angle::ANGLE_KIND == AngleKind::Radian) {
            return this->angle * (static_cast<T>(180.0) / Angle::PI);
        } else {
            return this->angle;
        }
    }

public:
    template <typename U, AngleKind CastKind = Kind>
    static constexpr Angle<U, CastKind> cast(const Angle& rhs) {
        using Output = Angle<U, CastKind>;
        if constexpr(CastKind == Angle::ANGLE_KIND) {
            return Output(static_cast<U>(rhs.angle));
        }
        if constexpr(CastKind == AngleKind::Radian) {
            return Output(static_cast<U>(rhs.to_radian_value()));
        } else {
            return Output(static_cast<U>(rhs.to_degree()));
        }
    }

    template <typename U, AngleKind CastKind = Kind>
    constexpr Angle<U, CastKind> to(void) const {
        using Output = Angle<U, CastKind>;
        if constexpr(CastKind == Angle::ANGLE_KIND) {
            return Output(static_cast<U>(this->angle));
        }
        if constexpr(CastKind == AngleKind::Radian) {
            return Output(static_cast<U>(this->to_radian_value()));
        } else {
            return Output(static_cast<U>(this->to_degree_value()));
        }
    }

public:
    template <typename U, AngleKind InputKind>
    constexpr explicit operator Angle<U, InputKind>(void) const {
        return Angle::cast<U, InputKind>(*this);
    }

    constexpr explicit operator float(void) const {
        return static_cast<float>(this->angle);
    }

    constexpr explicit operator double(void) const {
        return static_cast<double>(this->angle);
    }

    constexpr explicit operator long double(void) const {
        return static_cast<long double>(this->angle);
    }

public:
    constexpr void set(const T& rhs) {
        this->angle = rhs;
    };
    constexpr T get(void) const {
        return this->angle;
    };

public:
    template <AngleKind InputKind1, AngleKind InputKind2>
    constexpr void clamp(const Angle<T, InputKind1>& min, const Angle<T, InputKind2>& max) {
        this->angle = this->clamped(min, max).angle;
    }

    template <AngleKind InputKind1, AngleKind InputKind2>
    constexpr Angle clamped(const Angle<T, InputKind1>& min, const Angle<T, InputKind2>& max) const {
        return Angle(
            std::clamp(
                this->angle,
                Angle::cast<T, Kind>(min).angle,
                Angle::cast<T, Kind>(max).angle
            )
        );
    }

    template <AngleKind InputKind1, AngleKind InputKind2>
    static constexpr Angle min(
        const Angle<T, InputKind1>& lhs,
        const Angle<T, InputKind2>& rhs
    ) {
        return std::min(
            Angle::cast<T, Kind>(lhs).angle,
            Angle::cast<T, Kind>(rhs).angle
        );
    }

    template <AngleKind InputKind1, AngleKind InputKind2>
    static constexpr Angle max(
        const Angle<T, InputKind1>& lhs,
        const Angle<T, InputKind2>& rhs
    ) {
        return std::max(
            Angle::cast<T, Kind>(lhs).angle,
            Angle::cast<T, Kind>(rhs).angle
        );
    }

    constexpr Angle<T, AngleKind::Radian> sin(void) const {
        return Angle::Radian(std::sin(this->to_radian_value()));
    }

    constexpr Angle<T, AngleKind::Radian> cos(void) const {
        return Angle::Radian(std::cos(this->to_radian_value()));
    }

    constexpr Angle<T, AngleKind::Radian> tan(void) const {
        return Angle::Radian(std::tan(this->to_radian_value()));
    }

public:

    constexpr void operator=(const Angle& rhs) {
        this->angle = rhs.angle;
    };
    constexpr void operator+=(const Angle& rhs) {
        this->angle += rhs.angle;
    };
    constexpr void operator-=(const Angle& rhs) {
        this->angle -= rhs.angle;
    };

    constexpr void operator*=(const T& rhs) {
        this->angle *= rhs;
    };
    constexpr void operator/=(const T& rhs) {
        this->angle /= rhs;
    };

    template <AngleKind InputKind>
    constexpr Angle operator+(const Angle<T, InputKind>& rhs) const {
        return Angle(this->angle + Angle::cast<T, Kind>(rhs).angle);
    };
    template <AngleKind InputKind>
    constexpr Angle operator-(const Angle<T, InputKind>& rhs) const {
        return Angle(this->angle - Angle::cast<T, Kind>(rhs).angle);
    };

    constexpr Angle operator*(const T& rhs) const {
        return Angle(this->angle * rhs);
    };
    constexpr Angle operator/(const T& rhs) const {
        return Angle(this->angle / rhs);
    };

    template <AngleKind InputKind>
    constexpr bool operator<(const Angle<T, InputKind>& rhs) const {
        return this->angle < Angle::cast<T, Kind>(rhs).angle;
    }
    template <AngleKind InputKind>
    constexpr bool operator<=(const Angle<T, InputKind>& rhs) const {
        return this->angle <= Angle::cast<T, Kind>(rhs).angle;
    }
    template <AngleKind InputKind>
    constexpr bool operator>(const Angle<T, InputKind>& rhs) const {
        return this->angle > Angle::cast<T, Kind>(rhs).angle;
    }
    template <AngleKind InputKind>
    constexpr bool operator>=(const Angle<T, InputKind>& rhs) const {
        return this->angle >= Angle::cast<T, Kind>(rhs).angle;
    }
};

template <typename T>
using Radian = Angle<T, AngleKind::Radian>;
template <typename T>
using Degree = Angle<T, AngleKind::Degree>;
