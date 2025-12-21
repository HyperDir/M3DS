#pragma once

#include <cmath>

#include "../Spatial/Units.hpp"

namespace SPhys {
    struct Projection3D {
        Metres<float> from {};
        Metres<float> to {};

        [[nodiscard]] constexpr Metres<float> getOverlap(const Projection3D& with) const noexcept;
        [[nodiscard]] constexpr bool contains(const Projection3D& other) const noexcept;
    };

    constexpr Metres<float> Projection3D::getOverlap(const Projection3D& with) const noexcept {
        return std::min(to, with.to) - std::max(from, with.from);
    }

    constexpr bool Projection3D::contains(const Projection3D& other) const noexcept {
        return from <= other.from && to >= other.to;
    }
}
