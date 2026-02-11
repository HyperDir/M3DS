#pragma once

#include <numbers>

namespace M3DS {
    template <typename T> using Radians = T;
    template <typename T> using RadiansPerSecond = T;
    template <typename T> using RadiansPerSecondSquared = T;

    template <typename T> using Pixels = T;
    template <typename T> using PixelsPerSecond = T;
    template <typename T> using PixelsPerSecondSquared = T;

    template <typename T> using Millimetres = T;
    template <typename T> using Centimetres = T;
    template <typename T> using Decimetres = T;
    template <typename T> using Metres = T;
    template <typename T> using Kilometres = T;

    template <typename T> using MetresPerSecond = T;
    template <typename T> using MetresPerSecondSquared = T;
    template <typename T> using MetresPerSecondCubed = T;

    template <typename T> using Nanoseconds = T;
    template <typename T> using Microseconds = T;
    template <typename T> using Milliseconds = T;
    template <typename T> using Seconds = T;
    template <typename T> using Minutes = T;
    template <typename T> using Hours = T;
    template <typename T> using Days = T;

    template <typename T> using Milligrams = T;
    template <typename T> using Grams = T;
    template <typename T> using Kilograms = T;

    template <std::floating_point T>
    constexpr T deg2rad(const T deg) noexcept {
        return deg * (std::numbers::pi_v<T> / static_cast<T>(180.0));
    }

    template <std::floating_point T>
    constexpr T rad2deg(const T deg) noexcept {
        return deg * (static_cast<T>(180.0) / std::numbers::pi_v<T>);
    }
}
