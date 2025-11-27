#pragma once

#include <cmath>
#include <string>

#include <m3ds/utils/Debug.hpp>

#include <m3ds/spatial/Quaternion.hpp>
#include <m3ds/spatial/Vector2.hpp>
#include <m3ds/spatial/Vector3.hpp>

namespace M3DS {
    enum class InterpolationMethod : std::uint8_t {
        discrete,
        linear,
        square,
        cubic,
        quartic,
        sine,
        exponential
    };

    enum class Easing : uint8_t {
        none,
        in = 1,
        out = 1 << 1,
        in_out = in | out
    };

    constexpr Easing operator&(const Easing lhs, const Easing rhs) noexcept {
        return static_cast<Easing>(std::to_underlying(lhs) & std::to_underlying(rhs));
    }

    constexpr Easing operator|(const Easing lhs, const Easing rhs) noexcept {
        return static_cast<Easing>(std::to_underlying(lhs) | std::to_underlying(rhs));
    }

    template <typename T>
    struct FloatTypeImpl {
        using type = float;
    };

    template <typename U>
    struct FloatTypeImpl<Vec2<U>> {
        using type = Vec2<float>;
    };

    template <typename U>
    struct FloatTypeImpl<Vec3<U>> {
        using type = Vec3<float>;
    };

    template <typename T>
    using FloatType = FloatTypeImpl<T>::type;

    template <typename T>
    concept CanInterpolate = requires(T t) {
        { interpolate(t, t, float{}, InterpolationMethod{}, Easing{}) } -> std::convertible_to<T>;
    };


#define POWER_INTERPOLATE(power)                                                                            \
if (progress <= m) return static_cast<T>(fromFloat + (median - fromFloat) * std::pow(progress / m, power)); \
return static_cast<T>(toFloat + (median - toFloat) * std::pow((1.f - progress) / (1.f - m), power))

    template <typename T>
    concept interpolateable = requires(T a, T b, float f) {
        { a + b } -> std::convertible_to<T>;
        { a - b } -> std::convertible_to<T>;
        { a * f } -> std::convertible_to<T>;
    };

    template <interpolateable T>
    [[nodiscard]] constexpr T scale(
        const T value,
        const float by
    ) noexcept {
        if (by == 1.f) return value;
        return static_cast<T>(static_cast<FloatType<T>>(value) * by);
    }

    [[nodiscard]] constexpr Quaternion scale(
        const Quaternion& value,
        const float by
    ) noexcept {
        if (by == 1.f) return value;
        return SPhys::slerp(Quaternion{}, value, by);
    }

    template <typename T>
    [[nodiscard]] constexpr T scale(
        const T value,
        [[maybe_unused]] float by
    ) noexcept {
        Debug::warn("Unable to scale!");
        return value;
    }

    template <interpolateable T>
    [[nodiscard]] constexpr T combine(
        const T lhs,
        const T rhs
    ) noexcept {
        return lhs + rhs;
    }

    [[nodiscard]] constexpr Quaternion combine(
        const Quaternion& lhs,
        const Quaternion& rhs
    ) noexcept {
        return lhs * rhs;
    }

    template <typename T>
    [[nodiscard]] constexpr T combine(
        const T lhs,
        [[maybe_unused]] const T rhs
    ) noexcept {
        Debug::warn("Unable to combine!");
        return lhs;
    }

    template <interpolateable T>
    constexpr T interpolate(
        const T from,
        const T to,
        const float progress,
        const InterpolationMethod method = InterpolationMethod::linear,
        const Easing easing = Easing::in_out
    ) {
        if (progress <= 0.f) return from;
        if (progress >= 1.f) return to;

        const float m = [&easing] {
            if (easing == Easing::in) return 1.f;
            if (easing == Easing::in_out) return 0.5f;
            return 0.f;
        }();

        const FloatType<T> fromFloat = static_cast<FloatType<T>>(from);
        const FloatType<T> toFloat = static_cast<FloatType<T>>(to);

        if (easing == Easing::none || method == InterpolationMethod::linear)
            return static_cast<T>(fromFloat + (toFloat - fromFloat) * progress);

        const FloatType<T> median = fromFloat * (1.f - m) + toFloat * m;

        switch (method) {
            default:
                Debug::warn("Unsupported Interpolation Method!");
                break;
            case InterpolationMethod::discrete:
                return static_cast<T>(fromFloat);
            case InterpolationMethod::square:
                POWER_INTERPOLATE(2.f);
            case InterpolationMethod::cubic:
                POWER_INTERPOLATE(3.f);
            case InterpolationMethod::quartic:
                POWER_INTERPOLATE(4.f);
            case InterpolationMethod::exponential:
                if (progress <= m)
                    return static_cast<T>((median - fromFloat) * (std::cosh(1.3169578969f * progress / m) - 1.f) + fromFloat);

                return static_cast<T>((median - toFloat) * (std::cosh(1.3169578969f *  (1.f - progress) / (m - 1.f)) - 1.f) + toFloat);
            case InterpolationMethod::sine:
                const FloatType<T> a = (toFloat - fromFloat) / 2.f;
                if (m == 0.5f)
                    return static_cast<T>((fromFloat + toFloat) / 2.f - a * std::cos(std::numbers::pi_v<float> * progress));

                if (m == 0.f)
                    return static_cast<T>(fromFloat - a * 2.f * std::cos(std::numbers::pi_v<float> / 2.f * (progress + 1.f)));

                return static_cast<T>(toFloat - a * 2.f * std::cos(std::numbers::pi_v<float> / 2.f * progress));
        }
        return from;
    }

    constexpr Vector2i interpolate(
        const Vector2i& from,
        const Vector2i& to,
        const float progress,
        const InterpolationMethod method = InterpolationMethod::linear,
        const Easing easing = Easing::in_out
    ) {
        return static_cast<Vector2i>(
            interpolate(
                static_cast<Vector2>(from),
                static_cast<Vector2>(to),
                progress,
                method,
                easing
            )
        );
    }

    constexpr Vector3i interpolate(
        const Vector3i& from,
        const Vector3i& to,
        const float progress,
        const InterpolationMethod method = InterpolationMethod::linear,
        const Easing easing = Easing::in_out
    ) {
        return static_cast<Vector3i>(
            interpolate(
                static_cast<Vector3>(from),
                static_cast<Vector3>(to),
                progress,
                method,
                easing
            )
        );
    }


    template <typename T>
    constexpr Quat<T> interpolate(
        const Quat<T>& from,
        const Quat<T>& to,
        const float progress,
        [[maybe_unused]] const InterpolationMethod method = InterpolationMethod::linear,
        [[maybe_unused]] const Easing easing = Easing::in_out
    ) {
        if (progress <= 0) return from;
        if (progress >= 1) return to;

        return SPhys::slerp(from, to, progress);
    }
}
