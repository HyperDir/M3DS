#pragma once
#include "Units.hpp"

namespace SPhys {
    template <Arithmetic T>
    struct Quat;
}

#include "Vector3.hpp"

#include <cmath>
#include <iostream>

#include <type_traits>

namespace SPhys {
    template <Arithmetic T>
    struct Quat {
        T x{}, y{}, z{}, w = static_cast<T>(1);

        constexpr Quat() noexcept = default;
        constexpr Quat(T x, T y, T z, T w) noexcept;

        static constexpr Quat fromAxisAngle(const Vec3<T>& axis, Radians<T> angle) noexcept;
        static constexpr Quat fromAxisPair(const Vec3<T>& lhs, const Vec3<T>& rhs) noexcept;

        [[nodiscard]] constexpr bool operator==(const Quat& other) const noexcept = default;
        
        template <Arithmetic U>
        [[nodiscard]] explicit constexpr operator Quat<U>() const noexcept;

        [[nodiscard]] constexpr Quat normalise() const noexcept;

        template <Arithmetic U>
        [[nodiscard]] constexpr auto operator*(const Quat<U>& other) const noexcept;
        template <Arithmetic U>
        [[nodiscard]] constexpr Quat& operator*=(const Quat<U>& other) const noexcept;

        [[nodiscard]] constexpr Quat inverse() const noexcept;

        template <Arithmetic U>
        [[nodiscard]] constexpr auto dot(const Quat<U>& other) const noexcept;

        template <Arithmetic U>
        [[nodiscard]] constexpr auto rotate(const Vec3<U>& v) const noexcept;

        [[nodiscard]] constexpr auto lengthSquared() const noexcept;
    };

    using Quaternion = Quat<float>;


    template <Arithmetic T>
    constexpr Quat<T>::Quat(T x, T y, T z, T w) noexcept
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {}

    template <Arithmetic T>
    constexpr Quat<T> Quat<T>::fromAxisAngle(const Vec3<T>& axis, const Radians<T> angle) noexcept {
        const float halfAngle = angle / 2.0f;
        const float scale = std::sin(halfAngle);
        const Vec3<T> axisNormalised = axis.normalise();
        return {
            axisNormalised.x * scale,
            axisNormalised.y * scale,
            axisNormalised.z * scale,
            std::cos(halfAngle)
        };
    }

    template <Arithmetic T>
    constexpr Quat<T> Quat<T>::fromAxisPair(const Vec3<T>& lhs, const Vec3<T>& rhs) noexcept {
        const Vec3<T> a = lhs.cross(rhs);
        return Quat {
            a.x,
            a.y,
            a.z,
            std::sqrt(lhs.lengthSquared() * rhs.lengthSquared()) + lhs.dot(rhs)
        }.normalise();
    }

    template <Arithmetic T>
    template <Arithmetic U>
    constexpr Quat<T>::operator Quat<U>() const noexcept {
        return {
            static_cast<U>(x),
            static_cast<U>(y),
            static_cast<U>(z),
            static_cast<U>(w)
        };
    }

    template <Arithmetic T>
    constexpr Quat<T> Quat<T>::normalise() const noexcept {
        T length = std::sqrt(x * x + y * y + z * z + w * w);
        return {
            x / length,
            y / length,
            z / length,
            w / length
        };
    }

    template <Arithmetic T>
    template <Arithmetic U>
    constexpr auto Quat<T>::operator*(const Quat<U>& other) const noexcept {
        return Quat<std::common_type_t<T, U>> {
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w,
            w * other.w - x * other.x - y * other.y - z * other.z
        };
    }

    template <Arithmetic T>
    template <Arithmetic U>
    constexpr Quat<T>& Quat<T>::operator*=(const Quat<U>& other) const noexcept {
        return *this = {
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w,
            w * other.w - x * other.x - y * other.y - z * other.z
        };
    }

    template <Arithmetic T>
    constexpr Quat<T> Quat<T>::inverse() const noexcept {
        return { -x, -y, -z, w };
    }

    template <Arithmetic T>
    template <Arithmetic U>
    constexpr auto Quat<T>::dot(const Quat<U>& other) const noexcept {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    template <Arithmetic T>
    template <Arithmetic U>
    constexpr auto Quat<T>::rotate(const Vec3<U>& v) const noexcept {
        const Vec3 u { x, y, z };
        return 2.0f * u.dot(v) * u + (w * w - u.dot(u)) * v + 2.0f * w * u.cross(v);
    }

    template <Arithmetic T>
    constexpr auto Quat<T>::lengthSquared() const noexcept {
        return x * x + y * y + z * z + w * w;
    }


    template <Arithmetic T>
    constexpr Quat<T> slerp(const Quat<T>& from, const Quat<T>& to, const T t) {
        const auto dot = from.dot(to);

        const bool flip = dot < 0;
        const auto absDot = flip ? -dot : dot;

        if (absDot > 0.9995) {
            return Quat {
                from.x + t * ((flip ? -to.x : to.x) - from.x),
                from.y + t * ((flip ? -to.y : to.y) - from.y),
                from.z + t * ((flip ? -to.z : to.z) - from.z),
                from.w + t * ((flip ? -to.w : to.w) - from.w)
            }.normalise();
        }

        const auto theta = std::acos(absDot);
        const auto sinTheta = std::sin(theta);

        const auto w1 = std::sin((1 - t) * theta) / sinTheta;
        const auto w2 = std::sin(t * theta) / sinTheta;

        return Quat {
            from.x * w1 + (flip ? -to.x : to.x) * w2,
            from.y * w1 + (flip ? -to.y : to.y) * w2,
            from.z * w1 + (flip ? -to.z : to.z) * w2,
            from.w * w1 + (flip ? -to.w : to.w) * w2
        }.normalise();
    }

    template <Arithmetic T>
    constexpr Quat<T> nlerp(const Quat<T>& from, const Quat<T>& to, const T t) {
        return Quat<T> {
            from.x + t * (to.x - from.x),
            from.y + t * (to.y - from.y),
            from.z + t * (to.z - from.z),
            from.w + t * (to.w - from.w)
        }.normalise();
    }
}


template <typename T>
struct std::formatter<SPhys::Quat<T>> : std::formatter<T> {
    template <typename FormatContext>
    auto format(const SPhys::Quat<T>& quat, FormatContext& ctx) const {
        auto&& out = ctx.out();

        std::format_to(out, "(");

        std::formatter<T>::format(quat.x, ctx);
        std::format_to(out, ", ");
        std::formatter<T>::format(quat.y, ctx);
        std::format_to(out, ", ");
        std::formatter<T>::format(quat.z, ctx);
        std::format_to(out, ", ");
        std::formatter<T>::format(quat.w, ctx);

        return std::format_to(out, ")");
    }
};

template <typename T>
constexpr std::ostream& operator<<(std::ostream& stream, const SPhys::Quat<T>& vec) {
    std::format_to(std::ostreambuf_iterator{stream}, "{}", vec);
    return stream;
}