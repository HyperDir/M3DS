#pragma once

#include <type_traits>
#include <cmath>
#include <algorithm>

namespace SPhys {
    template <typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;

    template <Arithmetic T>
    [[nodiscard]] constexpr T lerp(const T from, const T to, const float progress) noexcept {
        return from + (to - from) * progress;
    }

    template <Arithmetic T>
    [[nodiscard]] constexpr T moveTowards(const T from, const T to, const float by) noexcept {
        if (from < to)
            return std::clamp(from + by, from, to);
        return std::clamp(from - by, to, from);
    }

    [[nodiscard]] constexpr auto efficientDistanceApproximation(std::floating_point auto from, std::floating_point auto to) noexcept {
        return std::abs(from - to);
    }

    template <typename T>
    requires std::is_integral_v<T>
    [[nodiscard]] constexpr auto efficientDistanceApproximation(T from, T to) noexcept {
        return std::abs(from - to);
    }

    template <typename T>
    [[nodiscard]] constexpr auto efficientDistanceApproximation(const T& from, const T& to) noexcept {
        return from.distanceSquaredTo(to);
    }

    [[nodiscard]] constexpr auto conversionAdd(Arithmetic auto lhs, Arithmetic auto rhs) noexcept {
        using CommonType = std::common_type_t<decltype(lhs), decltype(rhs)>;
        return static_cast<CommonType>(lhs) + static_cast<CommonType>(rhs);
    }

    [[nodiscard]] constexpr auto conversionSubtract(Arithmetic auto lhs, Arithmetic auto rhs) noexcept {
        using CommonType = std::common_type_t<decltype(lhs), decltype(rhs)>;
        return static_cast<CommonType>(lhs) - static_cast<CommonType>(rhs);
    }

    [[nodiscard]] constexpr auto conversionMultiply(Arithmetic auto lhs, Arithmetic auto rhs) noexcept {
        using CommonType = std::common_type_t<decltype(lhs), decltype(rhs)>;
        return static_cast<CommonType>(lhs) * static_cast<CommonType>(rhs);
    }

    [[nodiscard]] constexpr auto conversionDivide(Arithmetic auto lhs, Arithmetic auto rhs) noexcept {
        using CommonType = std::common_type_t<decltype(lhs), decltype(rhs)>;
        return static_cast<CommonType>(lhs) / static_cast<CommonType>(rhs);
    }

    template <std::floating_point T>
    [[nodiscard]] bool isEqualApprox(T lhs, T rhs, T epsilon = static_cast<T>(1e-5)) noexcept {
        return std::abs(lhs - rhs) <= epsilon;
    }

    template <std::floating_point T>
    [[nodiscard]] bool isZeroApprox(T lhs, T epsilon = static_cast<T>(1e-5)) noexcept {
        return std::abs(lhs) <= epsilon;
    }
}