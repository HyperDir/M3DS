#pragma once

#include <m3ds/lib/SPhys/Spatial/Arithmetic.hpp>

namespace M3DS {
    template <typename T> concept Arithmetic = SPhys::Arithmetic<T>;

    template <Arithmetic T>
    [[nodiscard]] constexpr T lerp(const T from, const T to, const float progress) noexcept {
        return from + (to - from) * progress;
    }
}